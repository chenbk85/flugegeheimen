#include "stdafx.h"

#include "kernel/kernel.h"
#include "kernel/request_builder.h"
#include "kernel/response_parser.h"
#include "helpers/socket.h"
#include "helpers/xml.h"
#include "devices/timer.h"
#include "devices/adc.h"




static void runTimerProc () {

	Flug::Xml config;
	Flug::Socket sock;
	Flug::RequestBuilder req;
	std::string addr, port;

	config.loadData("res/config.xml");
	config.get("/flugegeheimen/timer@addr", addr);
	config.get("/flugegeheimen/timer@port", port);
	sock.connect(addr, port);

	for (size_t i = 0; i < 8; i++) {
		std::string chanNo;
		std::stringstream ss;
		ss << i;
		ss >> chanNo;
		int chanDelay;
		config.get("/flugegeheimen/timer/channel_" + chanNo + "@delay", chanDelay);

		char res[4];
		req.flush();
		req << ((uint8_t) 0x80) << ((uint8_t) i) << ((uint8_t) 0x2) << ((uint16_t) chanDelay);
		sock.send(req.data(), req.size());

		sock.recv(reinterpret_cast<char *>(&res), sizeof(res));
		Flug::ResponseParser rparse(res, 4);
		uint8_t numReg, cmd;
		uint16_t val;
		rparse >> numReg >> cmd >> val;
		printf("Reg: %4d, Cmd: %4d, Val: % 6d\n", numReg, cmd, val);

	}

	sock.disconnect();

}

static void runTimerSetup () {
	Flug::Nsk::Timer timer;
	timer.loadConfig("res/config.xml");
	timer.connect();
	timer.setDefaultDelays();
	timer.disconnect();
}

static void runAdcSetup () {
	Flug::Nsk::Adc adc;
	adc.loadConfig("res/config.xml");
	adc.connect();
	adc.initDevice();
	adc.disconnect();
}

int main(int argc, char **argv) {

	if (argc < 2) {
		Flug::Kernel kern;
		kern.loadConfig("res/config.xml");
		kern.main();

	} else if (std::string(argv[1]) == "--timer") {

		runTimerSetup();

	} else if (std::string(argv[1]) == "--adc-init") {

		runAdcSetup();

	} else {
		std::cout << "Wrong option passed: \"" << argv[1] << "\"" << std::endl;
		return 1;
	}

	return 0;
}
