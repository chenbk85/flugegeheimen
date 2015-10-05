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
		Json::Value root;
		root["submodule"] = "devmgr";
		root["status"] = "success";
		int i = 0;
		std::cout << "Device enumeration" << std::endl;

		std::map<std::string, DeviceDriver*>::iterator iter;
		for (iter = m_devices.begin();
			 iter != m_devices.end(); iter++, i++) {
			root["devices_list"][i]["name"] = iter->first;
			std::cout  << "Device :: "<< iter->first << std::endl;
			if (iter->second->isOnline()) {
				root["devices_list"][i]["status"] = "online";
			} else {
				root["devices_list"][i]["status"] = "offline";
			}
		}

		std::cout << "Ended device enumeration" << std::endl;
		resp = root;
		return true;
	}

	void DeviceManager::registerDevice(const std::string &deviceName,
									   DeviceDriver *device) {
		m_devices[deviceName] = device;
	}

	DeviceDriver* DeviceManager::operator[](const std::string &deviceName) {
		if (m_devices.find(deviceName) != m_devices.end()) {
			return m_devices[deviceName];
		} else {
			return NULL;
		}
	}

}