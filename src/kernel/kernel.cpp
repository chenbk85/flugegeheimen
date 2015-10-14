#include "../stdafx.h"
#include "kernel.h"
#include "../devices/DummyDevice.h"
#include "../devices/AgilentOscope.h"
#include "../devices/NskSlowAdc.h"
#include "../devices/NskTimer.h"


namespace Flug {

	static const int MaxEventsNo = 10;

	Kernel::Kernel() {
		m_devmgr = new DeviceManager();
		m_dispatcher = new Dispatcher();
		m_monitor = new MonitorModule();
		m_deviceBuilder = new DeviceBuilder();
		m_devmgr->start();
		m_monitor->start();
	}

	Kernel::~Kernel() {
		m_gateway.disconnect();
	}

	void Kernel::registerModules() {
		m_dispatcher->registerModule("devmgr", m_devmgr);
		m_dispatcher->registerModule("monitor", m_monitor);
	}

	void Kernel::registerDrivers() {
		m_deviceBuilder->registerDeviceDriver<DummyDevice> ("DummyDevice");
		m_deviceBuilder->registerDeviceDriver<AgilentOscope> ("AgilentOscope");
		m_deviceBuilder->registerDeviceDriver<NskSlowAdc> ("NskSlowAdc");
		m_deviceBuilder->registerDeviceDriver<NskTimer> ("NskTimer");
	}

	void Kernel::registerDevices() {
		for (int i = 0; i < m_configuration["devices"].size(); i++) {
			std::string devName = m_configuration["devices"][i]["device"].asString();
			std::string drvName = m_configuration["devices"][i]["driver"].asString();

			std::cout << "Starting \"" << drvName << "\" instance \"" << devName << "\"" << std::endl;

			DeviceDriver * device = m_deviceBuilder->createDeviceInstance(drvName, devName);

			if (!device) {
				throw std::runtime_error("NULL module pointer");
			}
			device->loadConfig(m_configuration["devices"][i]["config"]);
			device->start();
			m_devmgr->registerDevice(devName, device);
			m_dispatcher->registerModule(devName, device);
		}
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
		m_gateway.bind(m_configuration["server"]["port"].asString());

		for (; 1;) {
			m_gateway.listen();
			std::cout << "Got new connection.." << std::endl;
			Socket sock(m_gateway.accept());
			sock.setNonblocking();
			m_pool.addSocket(sock);
		}
	}

	void Kernel::loadConfig(const std::string &confPath) {
		std::ostringstream contents;
		std::ifstream file(confPath);
		if (!file.is_open()) {
			throw std::runtime_error("Failed to load config");
		}
		contents << file.rdbuf();
		std::string confStr = contents.str();

		Json::Reader reader;
		reader.parse(confStr, m_configuration);
		std::cout << "Starting Flugegeheimen on " << m_configuration["server"]["port"].asString() << std::endl;
	}

	void Kernel::initialize(const std::string &configPath) {
		registerDrivers();
		loadConfig(configPath);
		registerModules();
		registerDevices();
	}

}
