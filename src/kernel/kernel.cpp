#include "../stdafx.h"
#include "kernel.h"
#include "../helpers/xml.h"
#include "../devices/DummyDevice.h"
#include "../devices/AgilentOscope.h"
#include "../devices/NskSlowAdc.h"
#include "RequestEnumerator.h"



namespace Flug {

	static const int MaxEventsNo = 10;

	Kernel::Kernel() {
		m_devmgr = new DeviceManager();
		m_dispatcher = new Dispatcher();
		m_monitor = new MonitorModule();
		m_devmgr->start();
		m_monitor->start();
	}

	Kernel::~Kernel() {
		m_gateway.disconnect();
	}

	void Kernel::registerModules() {
		m_dispatcher->registerModule("devmgr", m_devmgr);
		m_dispatcher->registerModule("monitor", m_monitor);
		registerDevice("DummyDevice", new DummyDevice());
		registerDevice("AgilentOscope", new AgilentOscope());
		registerDevice("NskSlowAdc", new NskSlowAdc());
	}

	void Kernel::registerDevice(const std::string &deviceName, DeviceDriver *device) {
		if (!device) {
			throw std::runtime_error("NULL module pointer");
		}
		device->start();
		m_devmgr->registerDevice(deviceName, device);
		m_dispatcher->registerModule(deviceName, device);
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
					if (m_dispatcher->hasModule(msg)) {
						if (!m_dispatcher->dispatchRequest(msg, pbuf)) {
							pbuf->sendMessage("\"status\":\"error\","
								  "\"description\":\"Failed to dispatch message\"");
						}
					} else {
						pbuf->sendMessage("\"status\":\"error\","
						  "\"description\":\"Failed to dispatch message\"");

					}
					//std::string res;
					//handleRequest(msg, res);
					//pbuf->sendMessage(res);
				} else {
					//std::cout << "#No message" << std::endl;
				}
			}

			Response resp;
			while (m_dispatcher->checkForResponses(resp)) {
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
			std::cout << "Got new connection.." << std::endl;
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
