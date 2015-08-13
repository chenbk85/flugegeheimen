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


		void handlingProc ();


	protected:
		void openGateway ();

		Socket m_gateway;
		ConnectionPool m_pool;
		std::string m_gatewayPort;
	private:
	};


}
