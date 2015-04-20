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

static void runAdcSetup (size_t deviceNo = 0) {
	Flug::Nsk::Adc adc;
	adc.loadConfig("res/config.xml", deviceNo);
	adc.connect();
	adc.initDevice();
	adc.disconnect();
	std::cout << "Loaded matrix to #" << deviceNo << std::endl;
}

static void runCrateCalibrationCheck () {
	Flug::Xml conf;
	conf.loadData("res/config.xml");
	size_t modulesCount;
	conf.get("/flugegeheimen/adc@modulesCount", modulesCount);

	/*std::cout << "Loading timer setup.. ";
	runTimerSetup();
	std::cout << " [OK]";*/

	uint16_t buf[Flug::Nsk::Adc::ADC_PAGE_SIZE/2];

	for (size_t i = 0; i < modulesCount; i++) {
		std::stringstream ss;
		std::string sno;
		std::cout << "Running ADC #" << i << " calibration.. ";
		Flug::Nsk::Adc adc;
		adc.loadConfig("res/config.xml", i);
		std::cout << " connecting.. ";
		adc.connect();
		std::cout << " configuring device.. ";
		adc.initDevice();

		std::cout << " reading data.. ";
		adc.readPage(reinterpret_cast<char*>(buf));
		ss << i;
		ss >> sno;
		std::ofstream file;
		std::cout << "data/calibra_" + sno + ".csv" << std::endl;
		file.open(("data/calibra_" + sno + ".csv").c_str(), std::ios::trunc);
		for (size_t j = 0; j < Flug::Nsk::Adc::ADC_PAGE_SIZE/2; j++) {
			file << buf[j] << std::endl;
		}
		file.close();
		std::cout << " [OK]" << std::endl;

		adc.disconnect();
	}

}

static void runCrateCalibrationCheckFast () {
	Flug::Xml conf;
	conf.loadData("res/config.xml");
	size_t modulesCount;
	conf.get("/flugegeheimen/adc@modulesCount", modulesCount);

	/*std::cout << "Loading timer setup.. ";
	runTimerSetup();
	std::cout << " [OK]";*/

	uint16_t buf[Flug::Nsk::Adc::ADC_PAGE_SIZE/2];

	for (size_t i = 0; i < modulesCount; i++) {
		std::stringstream ss;
		std::string sno;
		std::cout << "Running ADC #" << i << " calibration.. ";
		Flug::Nsk::Adc adc;
		adc.loadConfig("res/config.xml", i);
		std::cout << " connecting.. ";
		adc.connect();

		std::cout << " reading data.. ";
		adc.readPage(reinterpret_cast<char*>(buf));
		ss << i;
		ss >> sno;
		std::ofstream file;
		std::cout << "data/calibra_" + sno + ".csv" << std::endl;
		file.open(("data/calibra_" + sno + ".csv").c_str(), std::ios::trunc);
		for (size_t j = 0; j < Flug::Nsk::Adc::ADC_PAGE_SIZE/2; j++) {
			file << buf[j] << std::endl;
		}
		file.close();
		std::cout << " [OK]" << std::endl;

		adc.disconnect();
	}

}

static void runCrateCheck () {
	Flug::Xml conf;
	conf.loadData("res/config.xml");
	size_t modulesCount;
	conf.get("/flugegeheimen/adc@modulesCount", modulesCount);

	/*std::cout << "Loading timer setup.. ";
	runTimerSetup();
	std::cout << " [OK]";*/

	uint16_t buf[Flug::Nsk::Adc::ADC_PAGE_SIZE/2];
	std::stringstream ss;
	std::string sno;

	for (size_t i = 0; i < modulesCount; i++) {
		std::cout << "Running ADC #" << i << " calibration.. ";
		Flug::Nsk::Adc adc;
		adc.loadConfig("res/config.xml", i);
		std::cout << " connecting.. ";
		adc.connect();
		std::cout << " configuring device.. ";
		adc.initDevice();

		std::cout << " reading data.. ";
		adc.setPagesNumber(1024);
		adc.allowStart();
		adc.readPage(reinterpret_cast<char*>(buf));

		ss << i;
		ss >> sno;
		std::ofstream file;
		file.open(("data/data_" + sno + ".csv").c_str(), std::ios::trunc);
		for (size_t j = 0; j < Flug::Nsk::Adc::ADC_PAGE_SIZE/2; j++) {
			file << buf[j] << std::endl;
		}
		file.close();
		std::cout << " [OK]" << std::endl;

		adc.disconnect();
	}

}

static void runCrateCheckFast () {
	Flug::Xml conf;
	conf.loadData("res/config.xml");
	size_t modulesCount;
	conf.get("/flugegeheimen/adc@modulesCount", modulesCount);

	/*std::cout << "Loading timer setup.. ";
	runTimerSetup();
	std::cout << " [OK]";*/

	uint16_t buf[Flug::Nsk::Adc::ADC_PAGE_SIZE/2];
	std::stringstream ss;
	std::string sno;

	for (size_t i = 0; i < modulesCount; i++) {
		std::cout << "Running ADC #" << i << " calibration.. ";
		Flug::Nsk::Adc adc;
		adc.loadConfig("res/config.xml", i);
		std::cout << " connecting.. ";
		adc.connect();

		std::cout << " reading data.. ";
		adc.setPagesNumber(1024);
		adc.allowStart();
		adc.readPage(reinterpret_cast<char*>(buf));

		ss << i;
		ss >> sno;
		std::ofstream file;
		file.open(("data/data_" + sno + ".csv").c_str(), std::ios::trunc);
		for (size_t j = 0; j < Flug::Nsk::Adc::ADC_PAGE_SIZE/2; j++) {
			file << buf[j] << std::endl;
		}
		file.close();
		std::cout << " [OK]" << std::endl;

		adc.disconnect();
	}

}

static void runCheck () {
	Flug::Xml conf;
	conf.loadData("res/config.xml");
	size_t modulesCount, pagesNumber;
	conf.get("/flugegeheimen/adc@modulesCount", modulesCount);

	uint16_t buf[Flug::Nsk::Adc::ADC_PAGE_SIZE/2];

	for (size_t i = 0; i < modulesCount; i++) {
		std::stringstream ss;
		std::string sno;
		ss << i;
		ss >> sno;
		std::ofstream file;

		std::cout << "Running ADC #" << i << " calibration" << std::endl;
		Flug::Nsk::Adc adc;
		adc.loadConfig("res/config.xml", i);
		std::cout << " connecting" << std::endl;
		adc.connect();
		std::cout << " loading config" << std::endl;
		adc.setPagesNumber(0);
		//adc.setCounter(0);
		adc.allowStart();
		//adc.softStart();
		std::cout << " reading data" << std::endl;
		
		char c;
		c = getchar();

		adc.readPage(reinterpret_cast<char*>(buf));

		file.open(("data/data_" + sno + ".csv").c_str(), std::ios::trunc);
		for (size_t j = 0; j < Flug::Nsk::Adc::ADC_PAGE_SIZE/2; j++) {
			//file << htons(buf[j]) << std::endl;
			file << buf[j] << std::endl;
		}
		file.close();

		adc.disconnect();
	}
}

int main(int argc, char **argv) {

	if (argc < 2) {
		Flug::Kernel kern;
		kern.loadConfig("res/config.xml");
		kern.main();

	} else if (std::string(argv[1]) == "--timer") {

		runTimerSetup();

	} else if (std::string(argv[1]) == "--check") {

		runCheck();

	} else if (std::string(argv[1]) == "--adc-init") {

		//for (size_t i = 6; i < 8; i++) {
		//}
		std::thread thr1(runAdcSetup, 0);
		std::thread thr2(runAdcSetup, 1);
		std::thread thr3(runAdcSetup, 2);
		std::thread thr4(runAdcSetup, 3);
		std::thread thr5(runAdcSetup, 4);
		std::thread thr6(runAdcSetup, 5);
		std::thread thr7(runAdcSetup, 6);
		std::thread thr8(runAdcSetup, 7);

		thr1.join();
		thr2.join();
		thr3.join();
		thr4.join();
		thr5.join();
		thr6.join();
		thr7.join();
		thr8.join();

	} else {

		std::cout << "Wrong option passed: \"" << argv[1] << "\"" << std::endl;
		return 1;
	}

	return 0;
}
