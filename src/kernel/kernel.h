#pragma once

#include "../helpers/socket.h"
#include "pool.h"
#include "Dispatcher.h"
#include "DeviceManager.h"
#include "DeviceDriver.h"
#include "MonitorModule.h"
#include "ArchiveModule.h"
#include "DeviceBuilder.h"
#include "InterlockedArchiveBackend.h"
#include "Intercom.h"

namespace Flug {


	class Kernel {
	public:
		Kernel();

		virtual ~Kernel();

		void main();

		void initialize (const std::string & configPath);

	protected:


		void handlingProc();

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
		ArchiveModule * m_archive;
		MonitorModule * m_monitor;
		DeviceBuilder * m_deviceBuilder;
        InterlockedArchiveBackend * m_archBackend;
        Intercom & m_intercom;

	};


}
