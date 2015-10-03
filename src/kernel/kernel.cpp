#include "../stdafx.h"
#include "kernel.h"
#include "../helpers/xml.h"
#include "../devices/DummyDevice.h"
#include "RequestEnumerator.h"


namespace Flug {

	static const int MaxEventsNo = 10;

	Kernel::Kernel() {
		m_devmgr = new DeviceManager();
	}

	Kernel::~Kernel() {
	}

	void Kernel::registerModules() {
		m_dispatcher.registerModule("devmgr", m_devmgr);
	}

	void Kernel::handleRequest(const std::string &req, std::string &res) { //OLD AND UNUSED
		if (req == "getDummyData") {
			char data[0x100];
			DummyDevice device;
			device.getData(data, 0x100);
			std::string dataJson;
			dataToJsonArray(data, 0x100, dataJson);
			res = dataJson;
		} else {
			res = "{\"status\":\"success\",\"request\":"
				  + req + "}";
		}



		/*Request request(req);

		if (!request.m_parsed) {
			res = "{\"status\":\"error\",\"description\":\"Wrong JSON format\"}";
			return;
		}

		m_dispatcher.dispatchRequest(request);*/
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
			size_t evsNo = m_pool.wait(evs, MaxEventsNo); //THROTTLING OUT THERE?
			for (size_t evNo = 0; evNo < evsNo; evNo++) {
				PollingBuffer *pbuf = (PollingBuffer *) evs[evNo].data.ptr;

				pbuf->performTransimission(evs[evNo].events);

				std::string msg;
				if (pbuf->recvMessage(msg)) { //!check for recieved requests
					std::cout << "[" << msg << "]" << std::endl;
					m_dispatcher.dispatchRequest(msg, pbuf);
					//std::string res;
					//handleRequest(msg, res);
					//pbuf->sendMessage(res);
				} else {
					//std::cout << "#No message" << std::endl;
				}
			}

			Response resp;
			while (m_dispatcher.checkForResponses(resp)) {
				if (resp.m_pbuf) {
					resp.m_pbuf->sendMessage(resp.m_string);
				} else {
					throw std::runtime_error("Null pointer instead of pbuf");
				}
			}
		}
	}

	void Kernel::main() {
		std::thread handlingThread(&Kernel::handlingProc, this);
		m_gateway.bind(m_gatewayPort);

		for (; 1;) {
			m_gateway.listen();
			Socket sock(m_gateway.accept());
			sock.setNonblocking();
			m_pool.addSocket(sock);
		}
	}

	void Kernel::loadConfig(const std::string &confPath) {
		Xml xml;
		xml.loadData(confPath);
		xml.get("/flugegeheimen/server/net@port", m_gatewayPort);
		std::cout << "Starting Flugegeheimen on " << m_gatewayPort << std::endl;
	}

}
