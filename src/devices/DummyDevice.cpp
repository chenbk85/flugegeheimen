//
// Created by user on 8/14/15.
//

#include "../stdafx.h"
#include "DummyDevice.h"


namespace Flug {
	DummyDevice::DummyDevice() {
	}

	DummyDevice::~DummyDevice() {
	}

	void DummyDevice::getData(char *buf, size_t size) {
		for (size_t i = 0; i < size; i++) {
			buf[i] = (char) (rand() % 0xFF);
		}
	}


	bool DummyDevice::initModule() {
		srand(time(NULL));
		return true;
	}

	bool DummyDevice::destroyModule() {
		return true;
	}

	bool DummyDevice::handleRequest(Request &req, Response &resp) {
		if (req.m_json["request"].asString() != "getDummyData") {
			return false;
		}
		char data[0x100];
		getData(data, 0x100);
		std::string dataJson;
		dataToJsonArray(data, 0x100, dataJson);
		resp.m_string = dataJson;
		return true;
	}

	void DummyDevice::dataToJsonArray(const char *data, size_t size, std::string &jsonArray) {
		std::stringstream ss;
		ss << "[";
		for (size_t i = 0; i < size; i++) {
			ss << (int16_t) data[i];
			if (i != size - 1)
				ss << ",";
		}
		ss << "]";
		jsonArray = ss.str();
	}

	bool DummyDevice::isOnline() {
		return true;
	}

}