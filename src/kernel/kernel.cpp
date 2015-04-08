#include "../stdafx.h"
#include "kernel.h"

namespace Flug {


Kernel::Kernel () {
}

Kernel::~Kernel () {
}

void Kernel::main () {
	while (1) {
		std::cout << "In main loop" << std::endl;
		sleep(1);
	}
}

void Kernel::loadConfig (const std::string & confPath) {
	std::ifstream testFile(confPath.c_str(), std::ios::binary);
	std::vector<char> fileContents((std::istreambuf_iterator<char>(testFile)),
									std::istreambuf_iterator<char>());
	rapidxml::xml_document<> conf;
	conf.parse<0> (fileContents.data());

	std::cout << conf.first_node("flugegeheimen")->first_node("server")->first_node("net")->first_attribute("port")->value() << std::endl;
}


}
