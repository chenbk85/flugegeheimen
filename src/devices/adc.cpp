#include "../stdafx.h"
#include "adc.h"

#include "../kernel/request_builder.h"
#include "../kernel/response_parser.h"

namespace Flug {
	namespace Nsk {
		Adc::Adc () {

		}

		Adc::~Adc () {

		}

		void Adc::loadConfig(const std::string &xmlPath, size_t deviceNo) {
			m_config.loadData(xmlPath);
			m_config.get("/flugegeheimen/adc/firmware@path", m_firmwarePath);
			m_config.get("/flugegeheimen/adc/net@info_port", m_dataPort);
			m_config.get("/flugegeheimen/adc/net@control_port", m_controlPort);

			std::stringstream ss;
			ss << deviceNo;
			std::string n;
			ss >> n;
			m_config.get("/flugegeheimen/adc/devices/adc_" + n + "@addr", m_addr);
		}

		void Adc::initDevice() {
			uint8_t buf[ADC_PAGE_SIZE];
			loadFirmware();
			clearRegisters();
			initPll();
			allowStart();
			softStart();
			setCounter(0);
			readPage(buf);
		}

		void Adc::loadFirmware() {
			Flug::RequestBuilder req;
			Flug::ResponseParser res;
			uint8_t buf[1024];
			m_firmware.load(m_firmwarePath);
			uint32_t fwSize = m_firmware.size();
			req.flush();
			req << ((uint8_t)0xFE) << fwSize;
			m_controlSocket.sendData(req.data(), req.size());

			size_t chunk;
			for (size_t sentFw = 0; sentFw < fwSize; sentFw += chunk) {
				chunk = std::min(fwSize - sentFw, (size_t)1024);
				req.flush();
				(req << ((uint8_t)0xFF)).push(m_firmware.data() + sentFw, chunk);
				m_controlSocket.sendData(req.data(), req.size());
				m_controlSocket.recvData(buf, 1);
				if (*buf != 0x2E)
					throw std::runtime_error("Wrong return code on chunk upload attempt");
			}
		}

		void Adc::clearRegisters() {
			setRegister(ADC_REG_CLEAR, ADC_REGOP_WRITE, 0x01, 0x00);
		}

		void Adc::initPll() {
			setRegister(ADC_REG_PLL, ADC_REGOP_WRITE, 0x01, 0x00);
		}

		void Adc::allowStart() {
			setRegister(ADC_REG_ENABLE_START, ADC_REGOP_WRITE, 0x81, 0x00);
		}

		void Adc::softStart() {
			setRegister(ADC_REG_SOFT_START, ADC_REGOP_WRITE, 0x01, 0x00);
			sleep(1);
		}

		void Adc::setCounter(uint16_t counter) {
			setRegister(ADC_REG_ADDR, ADC_REGOP_WRITE, counter);
		}

		void Adc::setOffset(uint16_t offset) {
			setRegister(ADC_REG_OFFSET, ADC_REGOP_WRITE, offset);
		}

		void Adc::setPagesNumber(uint16_t pages) {
			setRegister(ADC_REG_PAGES_NUM, ADC_REGOP_WRITE, pages);
		}

		uint16_t Adc::getRegister (Register reg) {
			RequestBuilder req;
			ResponseParser res;
			uint8_t buf[1024];
			uint8_t cmd, numReg;
			uint16_t val;

			req << ((uint8_t)ADC_CONTROLL_CMD) << ((uint8_t)reg) << ((uint8_t)ADC_REGOP_READ) << ((uint16_t)0x0);
			m_controlSocket.sendData(req.data(), req.size());
			m_controlSocket.recvData(buf, 4);
			res.setData(buf, 4);
			res >> numReg >> cmd >> val;
			if (numReg != reg || cmd != ADC_REGOP_READ) {
				throw std::runtime_error("Returned wrong register or wrong command");
			}

			return val;
		}

		void Adc::setRegister (Register reg, RegOperstion regOp, uint8_t valLo, uint8_t valHi) {
			RequestBuilder req;
			ResponseParser res;
			uint8_t buf[1024];
			uint8_t cmd, numReg;
			uint16_t val;

			req << ((uint8_t)ADC_CONTROLL_CMD) << ((uint8_t)reg) << ((uint8_t)regOp) << valLo << valHi;
			m_controlSocket.sendData(req.data(), req.size());
			m_controlSocket.recvData(buf, 4);
			res.setData(buf, 4);
			res >> numReg >> cmd >> val;
		}

		void Adc::setRegister (Register reg, RegOperstion regOp, uint16_t val) {
			RequestBuilder req;
			ResponseParser res;
			uint8_t buf[1024];
			uint8_t cmd, numReg;
			uint16_t rval;

			req << ((uint8_t)ADC_CONTROLL_CMD) << ((uint8_t)reg) << ((uint8_t)regOp) << val;
			m_controlSocket.sendData(req.data(), req.size());
			m_controlSocket.recvData(buf, 4);
			res.setData(buf, 4);
			res >> numReg >> cmd >> rval;
		}
		void Adc::connect() {
			m_controlSocket.connect(m_addr, m_controlPort);
			m_dataSocket.connect(m_addr, m_dataPort);
		}

		void Adc::disconnect() {
			m_controlSocket.disconnect();
			m_dataSocket.disconnect();
		}

		void Adc::readPage(uint8_t *buf) {
			RequestBuilder req;
			req << ((uint8_t)0x10);
			m_dataSocket.sendData(req.data(), req.size());
			m_dataSocket.recvData(buf, ADC_PAGE_SIZE/2);
			m_dataSocket.sendData(req.data(), req.size());
			m_dataSocket.recvData(buf+ADC_PAGE_SIZE/2, ADC_PAGE_SIZE/2);

			const uint16_t * data = reinterpret_cast<const uint16_t *>(buf);
			/*for (size_t i = 0; i < ADC_PAGE_SIZE / 2; i++) {
				std::cout << data[i];
			}
			std::cout << std::endl;*/
		}
	}
}
