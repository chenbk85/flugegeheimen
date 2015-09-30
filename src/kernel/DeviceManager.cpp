//
// Created by user on 9/29/15.
//
#include "../stdafx.h"
#include "DeviceManager.h"


namespace Flug {
	DeviceManager::DeviceManager() { }

	DeviceManager::~DeviceManager() { }

	bool DeviceManager::initModule() { }

	bool DeviceManager::destroyModule() { }

	bool DeviceManager::handleRequest(const std::string &request, std::string &response) {
		response ="{"
				"\"submodule\":\"devmgr\", "
				"\"status\":\"success\""
				"}";
	}
}