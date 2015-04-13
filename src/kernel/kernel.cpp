#include "../stdafx.h"
#include "kernel.h"
#include "../helpers/xml.h"

namespace Flug {


	Kernel::Kernel() {
	}

	Kernel::~Kernel() {
	}

	void Kernel::main() {
		while (1) {
			std::cout << "In main loop" << std::endl;
			sleep(1);
		}
	}

	void Kernel::loadConfig(const std::string &confPath) {
		Xml xml;
		xml.loadData("res/config.xml");
		uint16_t port;
		xml.get("/flugegeheimen/server/net@port", port);
		std::cout << port << std::endl;
	}


}
