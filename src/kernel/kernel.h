#pragma once

#include "../helpers/socket.h"
#include "pool.h"

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


	protected:
		void openGateway();

		Socket m_gateway;
		ConnectionPool m_pool;
		std::string m_gatewayPort;
	private:
	};


}
