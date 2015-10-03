//
// Created by user on 9/29/15.
//
#include "../stdafx.h"
#include "DeviceManager.h"


namespace Flug {
	DeviceManager::DeviceManager() { }

	DeviceManager::~DeviceManager() { }

	bool DeviceManager::initModule() {
		return true;
	}

	bool DeviceManager::destroyModule() {
		return true;
	}

	bool DeviceManager::handleRequest(Request & req, Response & resp) {
		resp.m_string ="{"
				"\"submodule\":\"devmgr\", "
				"\"status\":\"success\""
				"}";
		return true;
	}
}