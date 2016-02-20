//
// Created by user on 23.10.15.
//

#include "../stdafx.h"
#include "NskFastAdc.h"

#include "../kernel/request_builder.h"
#include "../kernel/response_parser.h"
#include "../kernel/JsonBson.h"

#define CMD_HEADER ((uint8_t)0x80)
#define CMD_READ ((uint8_t)0x0)
#define CMD_WRITE ((uint8_t)0x2)
#define CMD_RESP_SIZE 0x4

#define REG_OFFSET 0x5
#define REG_PAGE 0x6
#define REG_PAGES_COUNT 0x7

#define NA_CLRREG_REG ((uint8_t)0x1)

#define FW_LOAD_CHUNK 0x400
#define DATA_PAGE_SIZE 0x400
#define DATA_SERIES_LENGTH 0x400
#define MAX_PAGES_COUNT 0x400

namespace Flug {

    static void LogBinary(const uint8_t *data, size_t size) {
        printf("%4lu: ", size);
        for (size_t i = 0; i < size; i++) {
            printf("0x%02x ", data[i]);
        }
        std::cout << std::endl;
    }

    NskFastAdc::NskFastAdc(const std::string &deviceInstanceName, const std::string &devType) :
            DeviceDriver(deviceInstanceName, devType), m_data(MAX_PAGES_COUNT * DATA_SERIES_LENGTH) {
    }

    NskFastAdc::~NskFastAdc() {
    }

    bool NskFastAdc::initModule() {
        m_infoSock.connect(m_addr, m_infoPort);
        m_controlSock.connect(m_addr, m_controlPort);
        m_pagesCount = 1;

		if (!m_manual) {
			loadFirmware();
			clearRegisters();
			initPll();

			//Calibra
			writeRegister(0x4, 0x0085);
			softStart();

			sleep(1);
			dropPage();
			setOffset(0x800);
			readDataPage(m_data.data());
			dropPage();
		}

        return true;
    }

    bool NskFastAdc::destroyModule() {
        m_infoSock.disconnect();
        m_controlSock.disconnect();
        return true;
    }

    bool NskFastAdc::loadConfig(Json::Value &config) {
        m_controlPort = config["controlPort"].asString();
        m_infoPort = config["infoPort"].asString();
        m_addr = config["addr"].asString();
        m_firmwareLocation = config["firmware"].asString();
		m_manual = config["manual"].asBool();

        std::ifstream file(m_firmwareLocation, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error(("Failed to load firmware data for NskSlowAdc " + m_addr).c_str());
        }
        m_firmware.assign((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());

        return true;
    }

	bool NskFastAdc::handleGetConfig (Request &req, Response &resp) {
		Json::Value root;
		root["status"] = "success";
		root["manual"] = m_manual;
		root["addr"] = m_addr;
		root["controlPort"] = m_controlPort;
		root["infoPort"] = m_infoPort;
		root["firmware"] = m_firmwareLocation;
	}

    bool NskFastAdc::handleRequest(Request &req, Response &resp) {
        std::string reqtype = req.m_json["reqtype"].asString();

        if (reqtype == "getData") {
            return false;//handleGetData(req, resp);
        } else if (reqtype == "setPagesCount") {
            return handleSetPagesCount(req, resp);
        } else if (reqtype == "waitForTrigger") {
            return handleWaitForTrigger(req, resp);
        } else if (reqtype == "softStart") {
            return handleSoftStart(req, resp);
        } else if (reqtype == "calibrate") {
            return handleCalibration(req, resp);
        } else if (reqtype == "downloadData") {
            return handleDownloadData(req, resp);
        } else if (reqtype == "setOffset") {
            return handleSetOffset (req, resp);
        } else if (reqtype == "ping") {
            return handlePing(req, resp);
        } else if (reqtype == "loadFirmware") {
	    	return handleLoadFirmware(req, resp);
		} else if (reqtype == "getConfig") {
			return handleGetConfig (req, resp);
		}

        return false;
    }

	bool NskFastAdc::handleLoadFirmware (Request & req, Response & resp) {
		Json::Value root;
		try {
			loadFirmware();
			clearRegisters();
			initPll();
		} catch (const std::runtime_error & ex) {
			root["status"] = "error";
			root["description"] = ex.what();
			return true;
		}
		root["status"] = "success";
		resp = root;
		return true;
	}

    void NskFastAdc::loadFirmware() {
        uint32_t fwSize = m_firmware.size();
        if (fwSize == 0) {
            throw std::runtime_error("Firmware data not loaded");
        }

        RequestBuilder req;
        req << (uint8_t) 0xFE << fwSize;
        m_controlSock.sendData(req.data(), req.size());
        req.flush();

        uint8_t ret;
        m_controlSock.recvData(reinterpret_cast<uint8_t *>(&ret), 0x1);

        if (ret != 0x2E) {
            throw std::runtime_error("Firmware upload process terminated");
        }


        for (uint32_t loaded = 0; loaded < fwSize; loaded += FW_LOAD_CHUNK) {
            req << (uint8_t) 0xFF;
            req.push(m_firmware.data() + loaded,
                     std::min(fwSize - loaded, ((uint32_t)FW_LOAD_CHUNK)));
            m_controlSock.sendData(req.data(), req.size());
            req.flush();

            if (fwSize - loaded > FW_LOAD_CHUNK) {
                m_controlSock.recvData(reinterpret_cast<uint8_t *>(&ret), 0x1);

                if (ret != 0x2E) {
                    throw std::runtime_error("Firmware upload process terminated");
                }
            } else {
                uint16_t ret2;
                m_controlSock.recvData(reinterpret_cast<uint8_t *>(&ret2), 0x2);

                if (ret2 != 0x2E2E) {
                    throw std::runtime_error("Firmware upload process terminated");
                }
            }
        }
    }

    uint16_t NskFastAdc::readRegister(uint8_t reg) {
        uint16_t val;
        RequestBuilder req;
        req << CMD_HEADER << reg << CMD_READ << ((uint16_t) 0x0000);
        LogBinary(req.data(), req.size());
        m_controlSock.sendData(req.data(), req.size());
        req.flush();

        uint8_t buf[CMD_RESP_SIZE];
        m_controlSock.recvData(buf, CMD_RESP_SIZE);
        ResponseParser resp;
        resp.setData(buf, CMD_RESP_SIZE);
        LogBinary(resp.data(), resp.size());
        uint8_t regNo, commandType;

        resp >> regNo >> commandType >> val;

        if (regNo != reg || commandType != CMD_READ) {
            throw std::runtime_error("FastAdc: can't perform read of register");
        }

        return val;
    }

    void NskFastAdc::writeRegister(uint8_t reg, uint16_t val) {
        RequestBuilder req;
        req << CMD_HEADER << reg << CMD_WRITE << val;
        LogBinary(req.data(), req.size());
        m_controlSock.sendData(req.data(), req.size());
        req.flush();


        uint8_t buf[CMD_RESP_SIZE];
        m_controlSock.recvData(buf, CMD_RESP_SIZE);
        ResponseParser resp;
        resp.setData(buf, CMD_RESP_SIZE);
        LogBinary(resp.data(), resp.size());
        uint8_t regNo, commandType;
        uint16_t u16;

        resp >> regNo >> commandType >> u16;

        if (regNo != reg || commandType != CMD_WRITE) {
            throw std::runtime_error("FastAdc: can't perform write of register");
        }
    }

    void NskFastAdc::clearRegisters() {
        writeRegister(0x1, 0x0001);
    }

    void NskFastAdc::initPll() {
        writeRegister(0x2, 0x0001);
    }

    void NskFastAdc::allowStart() {
        writeRegister(0x4, 0x0081); //0x0081?
    }

    void NskFastAdc::softStart() {
        writeRegister(0x3, 0x0001);
    }


    void NskFastAdc::dropPage() {
        writeRegister(REG_PAGE, 0x0000);
    }

    void NskFastAdc::setPage(uint16_t address) {
        writeRegister(REG_PAGE, address);
    }

    void NskFastAdc::setOffset(uint16_t offset) {
        writeRegister(REG_OFFSET, offset);
    }

    void NskFastAdc::setDataPagesCount(uint16_t pages) {
        if (pages < 1 || pages > 0x400) {
            throw std::runtime_error("Too much pages!");
        }
        writeRegister(REG_PAGES_COUNT, pages - 1);
        m_pagesCount = pages;
    }

    uint16_t NskFastAdc::getOffset() {
        return readRegister(REG_OFFSET);
    }

    uint16_t NskFastAdc::getPage() {
        return readRegister(REG_PAGE);
    }

    uint16_t NskFastAdc::getPagesCount() {
        return m_pagesCount;
    }


    void NskFastAdc::getReadyForStart() {
        dropPage();
        allowStart();
        softStart();
    }

    void NskFastAdc::readDataPage(uint16_t *data) {
        uint8_t readCmd = 0x10;
        m_infoSock.sendData(reinterpret_cast<const uint8_t *>(&readCmd), 0x1);
        m_infoSock.recvData(reinterpret_cast<uint8_t *>(data), DATA_PAGE_SIZE);
        m_infoSock.sendData(reinterpret_cast<const uint8_t *>(&readCmd), 0x1);
        m_infoSock.recvData(reinterpret_cast<uint8_t *>(data) + DATA_PAGE_SIZE, DATA_PAGE_SIZE);
    }

    uint16_t NskFastAdc::readAllPages() {
        uint16_t pagesCount = getPagesCount();

        std::vector<uint16_t> tmpData(DATA_SERIES_LENGTH);


        for (int page = 0; page < pagesCount; page++) {
            readDataPage(tmpData.data());

            for (int i = 0; i < DATA_SERIES_LENGTH; i++) {
                m_data[i + page * DATA_PAGE_SIZE] = tmpData[i];
            }
        }

        return pagesCount;
    }

    bool NskFastAdc::handleGetData(Request &req, Response &resp) {
        Json::Value root;
        root["status"] = "success";
        root["xincr"] = 1;

        getReadyForStart();
        sleep(1);

        uint16_t pages = readAllPages();

        for (int i = 0; i < pages * DATA_PAGE_SIZE; i++) {
            root["data"][0][i] = m_data[i];
        }

        resp = root;
        return true;
    }

    bool NskFastAdc::handleSetPagesCount(Request &req, Response &resp) {
        Json::Value root;
        root["status"] = "success";

        setDataPagesCount(req.m_json["pagesCount"].asUInt());

        resp = root;
        return true;
    }

    bool NskFastAdc::handleSoftStart(Request &req, Response &resp) {
        Json::Value root;
        root["status"] = "success";

        softStart();

        resp = root;
        return true;
    }

    bool NskFastAdc::handleWaitForTrigger(Request &req, Response &resp) {
        Json::Value root;
        root["status"] = "success";

        dropPage();
        allowStart();

        resp = root;
        return true;
    }

    bool NskFastAdc::handleDownloadData(Request &req, Response &resp) {
        Json::Value root;
        root["status"] = "success";
        root["xincr"] = 1;

        dropPage();

        uint16_t pages = readAllPages();

        for (int i = 0; i < pages * DATA_PAGE_SIZE; i++) {
            root["data"][0][i] = m_data[i];
        }

        resp = root;
        return true;
    }

    bool NskFastAdc::handleCalibration(Request &req, Response &resp) {
        Json::Value root;
        root["status"] = "success";
        clearRegisters();
        initPll();

        //Calibra
        writeRegister(0x4, 0x0085);
        softStart();

        usleep(100);
        dropPage();
        setOffset(0x800);
        resp = root;
        return true;
    }

    bool NskFastAdc::handleSetOffset(Request &req, Response &resp) {
        Json::Value root;
        root["status"] = "success";

        dropPage();
        uint16_t offset = req.m_json["offset"].asUInt();
        setOffset(offset);

        resp = root;
        return true;
    }

    bool NskFastAdc::handlePing(Request &req, Response &resp) {
        Json::Value root;
        root["status"] = "success";
        JsonBson data = std::string("[]");
        root["data"][0] = (Json::Value)data;
        resp = root;
        return true;
    }
}
