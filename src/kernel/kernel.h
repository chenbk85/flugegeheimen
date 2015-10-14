#pragma once

#include "../helpers/socket.h"
#include "pool.h"
#include "Dispatcher.h"
#include "DeviceManager.h"
#include "DeviceDriver.h"
#include "MonitorModule.h"
#include "DeviceBuilder.h"

namespace Flug {


	class Kernel {
	public:
		Kernel();

		virtual ~Kernel();

		void main();

		void initialize (const std::string & configPath);

	protected:


		void handlingProc();

		void handleRequest(const std::string &req, std::string &res);

		void loadConfig(const std::string &confPath);
		void registerModules();
		void registerDrivers ();
		void registerDevices ();



	private:
		Json::Value m_configuration;
		Socket m_gateway;
		ConnectionPool m_pool;
		Dispatcher * m_dispatcher;
		DeviceManager * m_devmgr;
		MonitorModule * m_monitor;
		DeviceBuilder * m_deviceBuilder;

	};


}
