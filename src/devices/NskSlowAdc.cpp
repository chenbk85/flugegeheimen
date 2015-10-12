//
// Created by user on 10/8/15.
//

#include "../stdafx.h"
#include "NskSlowAdc.h"
#include "../kernel/BinBuilder.h"

#define REG_HIVOLTAGE 0x5
#define REG_PAGES_COUNT 0x8
#define REG_COMMAND 0x4


#define VAL_HIVOLTAGE 0x3
#define VAL_DROP_ADDR 0x10
#define VAL_ALLOW_READ 0x8
#define VAL_ALLOW_START 0x2

namespace Flug {
	NskSlowAdc::NskSlowAdc() {
	}

	NskSlowAdc::~NskSlowAdc() { }

	bool NskSlowAdc::initModule() {
		return true;
	}

	bool NskSlowAdc::handleRequest(Flug::Request &req, Flug::Response &resp) {
		return true;
	}

	bool NskSlowAdc::destroyModule() {
		disconnect();
		return true;
	}

	bool NskSlowAdc::connect() {
		try {
			m_infoSock.connect("192.168.0.110", "2000");
			m_controlSock.connect("192.168.0.110", "2001");
			m_adcSock.connect("192.168.0.110", "2002");
		} catch (std::runtime_error & err) {
			std::cout << err.what() << std::endl;
			return false;
		}
	}

	void NskSlowAdc::disconnect() {
		m_infoSock.disconnect();
		m_adcSock.disconnect();
		m_controlSock.disconnect();
	}

	bool NskSlowAdc::performCommand(const Command &cmd, Response &resp) {
		BinBuilder commandBuilder;
		commandBuilder << (uint8_t) 0x80 << cmd.m_register << (uint8_t) 0x2 << cmd.m_value;
		std::vector<uint8_t> command;
		commandBuilder.getData(command);
		m_controlSock.sendData((const char *)command.data(), command.size());

		BinBuilder respBuilder;
		std::vector<uint8_t> response(0x4);
		m_controlSock.recvData((char*)response.data(), 0x4);
		respBuilder.setData(response.data(), 0x4);

		uint8_t unused;
		respBuilder >> resp.m_register >> unused >> resp.m_value;
		return true;
	}

	bool NskSlowAdc::setHighVoltage() {
		Command cmd;
		Response resp;
		cmd.m_register = REG_HIVOLTAGE;
		cmd.m_value = VAL_HIVOLTAGE;


	}




}