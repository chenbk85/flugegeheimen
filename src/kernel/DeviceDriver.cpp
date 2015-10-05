//
// Created by user on 9/30/15.
//

#include "../stdafx.h"
#include "DeviceDriver.h"

namespace Flug {
	DeviceDriver::DeviceDriver() { }

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
}