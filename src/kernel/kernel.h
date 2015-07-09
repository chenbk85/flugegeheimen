#pragma once

#include "../helpers/socket.h"

namespace Flug {


	class Kernel {
	public:
		Kernel();

		virtual ~Kernel();

		void main();

		void loadConfig(const std::string &confPath);


	protected:
		void openGateway ();

		Socket m_gateway;
		Socket m_sink;
		std::string m_gatewayPort;
	private:
	};


}
