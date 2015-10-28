//
// Created by user on 10/8/15.
//

#include "../stdafx.h"
#include "NskSlowAdc.h"
#include "../kernel/request_builder.h"
#include "../kernel/response_parser.h"

#define REG_HIVOLTAGE 0x5
#define REG_PAGES_COUNT 0x8
#define REG_COMMAND 0x4
#define REG_UNKNOWN_6 0x6


#define VAL_HIVOLTAGE_ON 0x3
#define VAL_HIVOLTAGE_OFF 0x2
#define VAL_DROP_ADDR 0x10
#define VAL_ALLOW_READ 0x8
#define VAL_ALLOW_START 0x2
#define VAL_MAGIC_COMMAND 0x6
#define VAL_UNKNOWN_1 0x1

#define READ_DATA_COMMAND 0x10
#define DATA_PAGE_SIZE 0x400
#define DATA_SERIES_LENGTH 0x400

#define CHANNELS_COUNT 8

#define FW_LOAD_CHUNK ((uint32_t)0x400)

namespace Flug {

    void LogBinary(const uint8_t *data, size_t size) {
        printf("%4lu: ", size);
        for (size_t i = 0; i < size; i++) {
            printf("0x%02x ", data[i]);
        }
        std::cout << std::endl;
    }

    NskSlowAdc::NskSlowAdc(const std::string &deviceInstanceName, const std::string &devType) :
            DeviceDriver(deviceInstanceName, devType) {
    }

    NskSlowAdc::~NskSlowAdc() { }

    bool NskSlowAdc::initModule() {
        connect();
        loadFirmware();
        for (uint8_t i = 0; i < CHANNELS_COUNT; i++) {
            setHighVoltage(i, m_highVoltageEnabled);
        }

        dropAddrCounter();
        setPagesCount(0);
        allowStart();

        sleep(1);

        allowRead();
        dropAddrCounter();

        readDataPages();

        return true;
    }

    bool NskSlowAdc::handleRequest(Flug::Request &req, Flug::Response &resp) {
        std::string reqtype = req.m_json["reqtype"].asString();
        if (reqtype == "getPage") {
            return true;
        } else if (reqtype == "allowStart") {
            return handleAllowStart(req, resp);
        } else if (reqtype == "allowRead") {
            return handleAllowRead(req, resp);
        } else if (reqtype == "getData") {
            return handleGetData(req, resp);
        } else if (reqtype == "getAllData") {
            return handleGetAllData(req, resp);
        } else if (reqtype == "getAmpVoltages") {
            return handleGetAmpVoltages(req, resp);
        }
        return false;
    }

    bool NskSlowAdc::destroyModule() {
        disconnect();
        return true;
    }

    bool NskSlowAdc::connect() {
        try {
            m_infoSock.connect(m_addr, m_infoPort);
            m_controlSock.connect(m_addr, m_controlPort);
            m_dacSock.connect(m_addr, m_dacPort);
        } catch (std::runtime_error &err) {
            std::cout << err.what() << std::endl;
            return false;
        }
    }

    void NskSlowAdc::disconnect() {
        m_infoSock.disconnect();
        m_dacSock.disconnect();
        m_controlSock.disconnect();
    }

    bool NskSlowAdc::performCommand(Socket &sock, const Cmd &cmd, Resp &resp, uint8_t chanNo) {
        RequestBuilder commandBuilder;
        commandBuilder.flush();
        commandBuilder << (uint8_t) 0x80 << cmd.m_register << chanNo << cmd.m_value;
        m_controlSock.sendData(commandBuilder.data(), commandBuilder.size());

        std::vector<uint8_t> response(0x4);
        m_controlSock.recvData(response.data(), 0x4);
        ResponseParser respParser(response.data(), 0x4);

        uint8_t unused;
        respParser >> resp.m_register >> unused >> resp.m_value;
        return true;
    }

    void NskSlowAdc::setHighVoltage(uint8_t chan, bool enable) {
        Resp resp;
        performCommand(m_controlSock, Cmd(REG_HIVOLTAGE, (enable) ? VAL_HIVOLTAGE_ON : VAL_HIVOLTAGE_OFF), resp,
                       ((chan << 2) | 2));

        if (resp.m_register != REG_HIVOLTAGE) {
            throw std::runtime_error(("Failed to set power supply settings on " + m_addr).c_str());
        }
    }


    bool NskSlowAdc::loadConfig(Json::Value &config) {
        m_dacPort = config["dacPort"].asString();
        m_infoPort = config["infoPort"].asString();
        m_controlPort = config["controlPort"].asString();
        m_addr = config["addr"].asString();
        m_firmwareLocation = config["firmware"].asString();
        m_highVoltageEnabled = config["highVoltage"].asBool();

        std::ifstream file(m_firmwareLocation, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error(("Failed to load firmware data for NskSlowAdc " + m_addr).c_str());
        }
        m_firmware.assign((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());

        return true;
    }

    void NskSlowAdc::loadFirmware() {
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
                     std::min(fwSize - loaded, FW_LOAD_CHUNK));
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


        sleep(1);
    }

    void NskSlowAdc::setPagesCount(uint16_t pages) {
        if (pages > 64) {
            throw std::runtime_error("NskSlowAdc: wrong pagescount");
        }
        Resp resp;
        performCommand(m_controlSock, Cmd(REG_PAGES_COUNT, pages), resp);
        if (resp.m_register != REG_PAGES_COUNT) {
            throw std::runtime_error("NskSlowAdc: setPagesCount command not performed");
        }
        m_pagesCount = pages;
    }

    void NskSlowAdc::dropAddrCounter() {
        Resp resp;
        performCommand(m_controlSock, Cmd(REG_COMMAND, VAL_DROP_ADDR), resp);
        if (resp.m_register != REG_COMMAND) {
            throw std::runtime_error("NskSlowAdc: failed to drop addr counter");
        }
    }

    void NskSlowAdc::allowRead() {
        Resp resp;
        performCommand(m_controlSock, Cmd(REG_COMMAND, VAL_ALLOW_READ), resp);
        if (resp.m_register != REG_COMMAND) {
            throw std::runtime_error("NskSlowAdc: failed to allow read");
        }
    }

    void NskSlowAdc::allowStart() {
        Resp resp;
        performCommand(m_controlSock, Cmd(REG_COMMAND, VAL_ALLOW_START), resp);
        if (resp.m_register != REG_COMMAND) {
            throw std::runtime_error("NskSlowAdc: failed to allow start");
        }
    }

    void NskSlowAdc::readDataPage(uint16_t *data) {
        uint8_t readCmd = READ_DATA_COMMAND;
        m_infoSock.sendData(reinterpret_cast<const uint8_t *>(&readCmd), 0x1);
        m_infoSock.recvData(reinterpret_cast<uint8_t *>(data), DATA_PAGE_SIZE);

    }

    void NskSlowAdc::readDataPages() {
        std::vector<uint16_t> tmpData(CHANNELS_COUNT * DATA_SERIES_LENGTH);

        for (int i = 0; i < 0x10; i++) {
            readDataPage(tmpData.data() + DATA_PAGE_SIZE / sizeof(int16_t) * i);
        }

        for (int chan = 0; chan < CHANNELS_COUNT; chan++) {
            for (int i = 0; i < DATA_SERIES_LENGTH; i++) {
                m_data[chan][i] = ntohs(tmpData[i * CHANNELS_COUNT + chan]);
            }
        }
    }


    bool NskSlowAdc::handleAllowStart(Request &req, Response &resp) {
        Json::Value root;
        root["status"] = "success";

        setPagesCount(63);
        dropAddrCounter();
        allowStart();

        resp = root;
        return true;
    }


    bool NskSlowAdc::handleAllowRead(Request &req, Response &resp) {
        Json::Value root;
        root["status"] = "success";

        allowRead();
        dropAddrCounter();

        resp = root;
        return true;
    }

    bool NskSlowAdc::handleGetData(Request &req, Response &resp) {
        Json::Value root;
        root["status"] = "success";

        setPagesCount(0);
        DO_THE_MAGIC_BITCH();
        readDataPages();

        for (int chan = 0; chan < CHANNELS_COUNT; chan++) {
            for (int i = 0; i < DATA_SERIES_LENGTH; i++) {
                root["data"][chan][i] = m_data[chan][i];
            }
        }

        root["xincr"] = 1e-7;

        resp = root;
        return true;
    }

    void NskSlowAdc::DO_THE_MAGIC_BITCH() {
        Resp resp;
        performCommand(m_controlSock, Cmd(REG_COMMAND, VAL_DROP_ADDR), resp);
        performCommand(m_controlSock, Cmd(REG_COMMAND, VAL_ALLOW_START), resp);
        performCommand(m_controlSock, Cmd(REG_COMMAND, VAL_MAGIC_COMMAND), resp);
        performCommand(m_controlSock, Cmd(REG_COMMAND, VAL_MAGIC_COMMAND), resp);
        sleep(2);
        performCommand(m_controlSock, Cmd(REG_UNKNOWN_6, VAL_UNKNOWN_1), resp);
        performCommand(m_controlSock, Cmd(REG_UNKNOWN_6, VAL_UNKNOWN_1), resp);
        performCommand(m_controlSock, Cmd(REG_COMMAND, VAL_ALLOW_READ), resp);
        performCommand(m_controlSock, Cmd(REG_COMMAND, VAL_DROP_ADDR), resp);
    }

    bool NskSlowAdc::handleGetAllData(Request &req, Response &resp) {
        Json::Value root;
        root["status"] = "success";

        setPagesCount(63);
        DO_THE_MAGIC_BITCH();

        for (int chunk = 0; chunk < 64; chunk++) {
            readDataPages();

            for (int chan = 0; chan < CHANNELS_COUNT; chan++) {
                for (int i = 0; i < DATA_SERIES_LENGTH; i++) {
                    root["data"][chan][i + chunk * 0x400] = m_data[chan][i];
                }
            }
        }

        root["xincr"] = 1e-7;

        resp = root;
        return true;
    }

    bool NskSlowAdc::handleGetAmpVoltages(Request &req, Response &resp) {
        Json::Value root;
        root["status"] = "success";

        uint16_t data[4];
        float coefs[4] = {-3, 3, 2, 2};

        uint8_t readCmd = 0x12;
        m_dacSock.sendData(reinterpret_cast<const uint8_t *>(&readCmd), 0x1);
        m_dacSock.recvData(reinterpret_cast<uint8_t *>(data), 0x8);

        for (int i = 0; i < 4; i++) {
            root["voltages"][i] = ( data[i] / 7863.0 * coefs[i] );
        }

        resp = root;
        return true;
    }
}