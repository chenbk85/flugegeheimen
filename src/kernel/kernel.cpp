#include "../stdafx.h"
#include "kernel.h"
#include "../helpers/xml.h"
#include "PollingBuffer.h"
#include "../devices/DummyDevice.h"
//#include "../config.h"

namespace Flug {

	static const int MaxEventsNo = 10;

	Kernel::Kernel() {
	}

	Kernel::~Kernel() {
	}

	void Kernel::handleRequest(const std::string &req, std::string &res) {
		if (req == "getDummyData") {
			char data[0x100];
			DummyDevice device;
			device.getData(data, 0x100);
			std::string dataJson;
			dataToJsonArray(data, 0x100, dataJson);
			res = dataJson;
		} else {
			res = "{\"status\":\"success\",\"request\":\""
				  + req + "\"}";
		}
	}

	void Kernel::dataToJsonArray(const char *data, size_t size, std::string &jsonArray) {
		std::stringstream ss;
		ss << "[";
		for (size_t i = 0; i < size; i++) {
			ss << (int16_t) data[i];
			if (i != size - 1)
				ss << ",";
		}
		ss << "]";
		jsonArray = ss.str();
	}


	void Kernel::handlingProc() {
		epoll_event evs[MaxEventsNo];
		for (; 1;) {
			size_t evsNo = m_pool.wait(evs, MaxEventsNo);
			for (size_t evNo = 0; evNo < evsNo; evNo++) {
				PollingBuffer *pbuf = (PollingBuffer *) evs[evNo].data.ptr;

				//std::cout << "#Performin' transmission" << std::endl;
				pbuf->performTransimission(evs[evNo].events);
				//std::cout << "#Finished performin' transmission" << std::endl;

				std::string msg;
				if (pbuf->recvMessage(msg)) {
					std::cout << "[" << msg << "]" << std::endl;
					std::string res;
					handleRequest(msg, res);
					pbuf->sendMessage(res);
				} else {
//					std::cout << "#No message" << std::endl;
				}
			}
		}
	}

	void Kernel::main() {
		std::thread handlingThread(&Kernel::handlingProc, this);
		m_gateway.bind(m_gatewayPort);
		/*for (;1;) {
			m_gateway.listen();
			m_sink = m_gateway.accept();
			std::string req;
			for (;req != "Disconnect!";) {
				m_sink.recvString(req);
				if (req == "ping") {
					m_sink.sendString("Yuppi! It works, you freaking whore.");
				} else {
					m_sink.sendString("Something gone wrong: bad request.");
				}
			}
			m_sink.disconnect();
		}*/

		for (; 1;) {
			m_gateway.listen();
			Socket sock(m_gateway.accept());
			std::cout << "#Adding new connection to polling device . fd="
			<< (int) sock << std::endl;
			sock.setNonblocking();
			m_pool.addSocket(sock);
		}
	}

	void Kernel::loadConfig(const std::string &confPath) {
		Xml xml;
		xml.loadData("res/config.xml");
		xml.get("/flugegeheimen/server/net@port", m_gatewayPort);
		std::cout << "Started Flugegeheimen on " << m_gatewayPort << std::endl;
	}

	void Kernel::openGateway() {
		m_gateway.bind(m_gatewayPort);
	}


}
