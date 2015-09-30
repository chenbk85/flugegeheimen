#pragma once

#include "../helpers/socket.h"
#include "pool.h"
#include "Dispatcher.h"
#include "DeviceManager.h"

namespace Flug {


	class Kernel {
	public:
		Kernel();

		virtual ~Kernel();

		void main();

		void loadConfig(const std::string &confPath);

		void handlingProc();

		void handleRequest(const std::string &req, std::string &res);

		void dataToJsonArray(const char *data, size_t size, std::string &jsonArray);

		void registerModules();


	protected:

		std::string m_gatewayPort;
		Socket m_gateway;

		ConnectionPool m_pool;
		Dispatcher m_dispatcher;
		DeviceManager m_devmgr;
	private:
	};


}
