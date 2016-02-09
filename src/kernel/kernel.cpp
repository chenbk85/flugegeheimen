#include "../stdafx.h"
#include "kernel.h"
#include "../devices/DummyDevice.h"
#include "../devices/AgilentOscope.h"
#include "../devices/NskSlowAdc.h"
#include "../devices/NskTimer.h"
#include "../devices/NskFastAdc.h"
#include "../devices/NskCrate.h"
#include "../devices/DummyCrate.h"
#include "PythonModule.h"


namespace Flug {

	static const int MaxEventsNo = 10;

	Kernel::Kernel() {
        Py_Initialize();

        m_archBackend = new InterlockedArchiveBackend();
        m_devmgr = new DeviceManager();
        m_remoteDispatcher = new RemoteDispatcher();
        m_localDispatcher = new LocalDispatcher();
        LocalDispatcherPtr::get().setPointer(m_localDispatcher);
        m_monitor = new MonitorModule();
        m_deviceBuilder = new DeviceBuilder();
        m_archive = new ArchiveModule(m_archBackend);
        m_scheduler = new SchedulerModule("scheduler");

        m_devmgr->start();
        m_monitor->start();
        m_archive->start();
        m_scheduler->start();
	}

	Kernel::~Kernel() {
		m_gateway.disconnect();
	}

	void Kernel::registerModules() {
		m_remoteDispatcher->registerModule("devmgr", m_devmgr);
		m_remoteDispatcher->registerModule("monitor", m_monitor);
        m_remoteDispatcher->registerModule("database", m_archive);
        m_remoteDispatcher->registerModule("scheduler", m_scheduler);
        m_localDispatcher->registerModule("devmgr", m_devmgr);
        m_localDispatcher->registerModule("monitor", m_monitor);
        m_localDispatcher->registerModule("database", m_archive);
        m_localDispatcher->registerModule("scheduler", m_scheduler);
	}

	void Kernel::registerDrivers() {
		m_deviceBuilder->registerDeviceDriver<DummyDevice> ("DummyDevice");
		m_deviceBuilder->registerDeviceDriver<AgilentOscope> ("AgilentOscope");
		m_deviceBuilder->registerDeviceDriver<NskSlowAdc> ("NskSlowAdc");
		m_deviceBuilder->registerDeviceDriver<NskFastAdc>("NskFastAdc");
		m_deviceBuilder->registerDeviceDriver<NskTimer> ("NskTimer");
        m_deviceBuilder->registerDeviceDriver<NskCrate> ("NskCrate");
        m_deviceBuilder->registerDeviceDriver<DummyCrate> ("DummyCrate");
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
			m_remoteDispatcher->registerModule(devName, device);
            m_localDispatcher->registerModule(devName, device);
            m_archBackend->addDeviceArchive(devName, drvName);
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
					if (m_remoteDispatcher->hasModule(msg)) {
						if (!m_remoteDispatcher->dispatchRequest(msg, pbuf)) {
							pbuf->sendMessage("{\"status\":\"error\","
									"\"description\":\"Failed to dispatch message\"}");
						}
					} else {
						pbuf->sendMessage("{\"status\":\"error\","
                              "\"description\":\"Failed to dispatch message\"}");
					}
				} else {
					//std::cout << "#No message" << std::endl;
				}
			}

            Request req;
            while (m_localDispatcher->checkForRequests(req)) {
                if (m_localDispatcher->hasModule(req.m_string)) {
                    if (!m_localDispatcher->dispatchRequest(req)) {
                        throw std::runtime_error("Failed to dispatch local request");
                    }
                } else {
                    throw std::runtime_error("Failed to find module (local)");

                }
            }

			Response resp;
			while (m_remoteDispatcher->checkForResponses(resp)) {
				if (resp.m_pbuf) {
					resp.m_pbuf->sendMessage(resp.m_string);
				} else {
					throw std::runtime_error("Null pointer instead of pbuf");
				}
			}


            while (m_localDispatcher->checkForResponses(resp)) {
                if (resp.m_module) {
                    resp.m_module->pushLocalResponse(resp);
                } else {
                    throw std::runtime_error("Null pointer instead of module");
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

        m_configuration = JsonBson(confStr);
        m_archive->loadConfig(m_configuration["database"]);
        m_scheduler->loadConfig(m_configuration["scripts"]);

        std::cout << "Starting Flugegeheimen on " <<
                m_configuration["server"]["port"].asString() << std::endl;
	}

	void Kernel::initialize(const std::string &configPath) {
		registerDrivers();
        registerScriptLangs();
		loadConfig(configPath);
        m_archBackend->connect();
		registerModules();
		registerDevices();
	}

    void Kernel::registerScriptLangs() {
        m_scheduler->addLang<PythonModule>("python");
    }

}


