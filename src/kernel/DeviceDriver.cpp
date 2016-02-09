//
// Created by user on 9/30/15.
//

#include "../stdafx.h"
#include "DeviceDriver.h"

namespace Flug {
	DeviceDriver::DeviceDriver(const std::string & deviceInstance, const std::string & deviceType) :
	Module(deviceInstance), m_deviceName(deviceInstance), m_deviceType(deviceType) { }

	DeviceDriver::~DeviceDriver() { }

	bool DeviceDriver::handleRequest(Request &req, Response &resp) {
		Json::Value root;
		root["status"] = "error";
		root["description"] = "Tried to pass a message to DeviceDriver superclass";
		resp = root;
		return true;
	}

	bool DeviceDriver::initModule() {
		return true;
	}

	bool DeviceDriver::destroyModule() {
		return true;
	}

	bool DeviceDriver::loadConfig(Json::Value &config) {
		return true;
	}

	const std::string& DeviceDriver::getDeviceName() {
		return m_deviceName;
	}

	const std::string& DeviceDriver::getDeviceType() {
		return m_deviceType;
	}

    bool DeviceDriver::rebootModule() {
        if (destroyModule() && initModule()) {
            return true;
        }
        return false;
    }
}