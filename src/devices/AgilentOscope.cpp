//
// Created by user on 10/4/15.
//

#include "../stdafx.h"
#include "AgilentOscope.h"

#define INFINIIUM_BUF_SIZE ((size_t)(1024 * 10))

namespace Flug {
	AgilentOscope::AgilentOscope() {
		m_connected = false;
	}

	AgilentOscope::~AgilentOscope() {
	}


	bool AgilentOscope::initModule() {
		try {
			m_sock.connect("172.16.13.238", "5025");
			m_connected = true;
			return true;
		} catch (std::runtime_error &err) {
			std::cout << err.what() << std::endl;
			m_connected = false;
			return false;
		}
	}

	bool AgilentOscope::destroyModule() {
		return true;
	}

	bool AgilentOscope::handleRequest(Request &req, Response &resp) {
		Json::Value root;
		std::string reqtype = req.m_json["reqtype"].asString();

		Json::StyledWriter writer;
		std::cout << std::endl << writer.write(req.m_json) << std::endl;

		if (reqtype == "command") {
			bool res = command(req.m_json["request"].asString());
			if (!res) {
				root["status"] = "failure";
				root["description"] = "Can't perform command on Agilent device";
			} else {
				root["status"] = "success";
			}
		}
		if (reqtype == "request") {
			std::string response;
			bool res = request(req.m_json["request"].asString(), response);
			if (!res) {
				root["status"] = "failure";
				root["description"] = "Can't perform request to Agilent device";
			} else {
				root["status"] = "success";
				root["value"] = response;
			}
		}

		if (reqtype == "getData") {
			std::cout << "Started handling the getData() request" << std::endl;
			std::vector<int16_t> data;



			command(":SYSTEM:HEADER OFF");

			command(":ACQUIRE:MODE RTIME");
			command(":ACQUIRE:POINTS:AUTO OFF");
			command(":ACQUIRE:AVERAGE OFF");
			command(":ACQUIRE:INTERPOLATE OFF");
			command(":ACQUIRE:POINTS 500");
			command(":WAVEFORM:BYTEORDER LSBFIRST");
			command(":WAVEFORM:FORMAT WORD");
			//command(":WAVEFORM:POINTS 500");

			command(":TIMEBASE:SCALE 500us");
			command(":DIGITIZE CHANNEL1");
			command(":WAVEFORM:SOURCE CHAN1");
			std::string points;
			std::cout << "Requesting for waveform points" << std::endl;
			request(":WAVEFORM:POINTS?", points);
			std::cout << "Points number is set to " << points << std::endl;
			command(":WAVEFORM:DATA?");
			std::cout << "Sent the configuration, recieving data.." << std::endl;
			getWordData(data);
			std::cout << "Data recieved, putting to JSON: " << data.size() << " elements" << std::endl;


			for (int i = 0; i < data.size(); i++) {
				root["data"][i] = data[i];
			}

			root["data_size"] = (int) data.size();

			command(":CHAN1:DISPLAY ON");
			command(":RUN");

			root["status"] = "success";
		}

		//std::cout << std::endl << writer.write(root) << std::endl;

		std::cout << "Stringify JSON.. " << std::endl;
		resp = root;
		std::cout << "JSON ready, sending to supervisor" << std::endl;
		return true;
	}

	bool AgilentOscope::isOnline() {
		/*std::string str;
		if (request("*IDN?", str)) {
			std::cout << str << std::endl;
			return true;
		} else {
			return false;
		}*/

		return m_connected;
	}

	bool AgilentOscope::command(const std::string &str) {
		try {
			m_sock.sendLine(str);
			return true;
		} catch (std::runtime_error &err) {
			m_connected = false;
			std::cout << err.what() << std::endl;
			return false;
		}
	}

	bool AgilentOscope::request(const std::string &req, std::string &resp) {
		if (!command(req)) {
			return false;
		}

		char buf[INFINIIUM_BUF_SIZE];

		bool gotAllData = false;

		resp.clear();

		do {
			size_t size = m_sock.recv(buf, INFINIIUM_BUF_SIZE - 1);
			for (size_t i = 0; i < size; i++) {
				if (buf[i] == '\n')
					gotAllData = true;
			}
			resp.append(buf, size);
		} while (!gotAllData);

		return true;
	}


	size_t AgilentOscope::parseWordRespHeader(const char *buf,
											  size_t &dataStart) {
		if (buf[0] != '#') {
			throw std::runtime_error("InfiniiumConnection::parseWordRespHeader: \
					invalid header");
		}
		size_t digitsCount = (size_t) (buf[1] - '0');
		if (digitsCount < 0 || digitsCount > 9) {
			throw std::runtime_error("InfiniiumConnection::parseWordRespHeader: \
					wrong digitsCount");
		}

		size_t dataSize = 0;

		for (size_t i = 0; i < digitsCount; i++) {
			size_t d = (size_t) (buf[2 + i] - '0');
			if (d < 0 || d > 9) {
				throw std::runtime_error("InfiniiumConnection::parseWordRespHeader: \
						wrong digit");
			}
			dataSize = dataSize * 10 + d;
		}

		dataStart = 2 + digitsCount;

		return dataSize;

	}

	void AgilentOscope::parseWordsFromBuf(const char *buf, size_t size,
										  std::vector<int16_t> &data) {
		size_t i;
		size_t reminder = size % 2;
		size_t count = size / 2;
		const int16_t *ptr = reinterpret_cast<const int16_t *>(buf);
		data.assign(ptr, ptr + count);
	}

	void AgilentOscope::getWordData(std::vector<int16_t> &data) {
		char buf[INFINIIUM_BUF_SIZE];

		size_t dataStart;
		size_t recieved = m_sock.recv(buf, INFINIIUM_BUF_SIZE);
		size_t dataLength = parseWordRespHeader(buf, dataStart);
		size_t fullLength = dataStart + dataLength + 1;

		data.clear();

		std::vector<char> dataBuf(dataLength + 1);
		dataBuf.assign(buf + dataStart, buf + recieved);

		while (recieved < fullLength) {
			size_t ret = m_sock.recv(dataBuf.data() + recieved - dataStart,
									 fullLength - recieved);
			recieved += ret;
		}

		parseWordsFromBuf(dataBuf.data(), dataLength, data);

		if (dataBuf[dataLength] != '\n') {
			throw std::runtime_error("InfiniiumConnection::getWordData(): wrong message ending");
		}
	}

}