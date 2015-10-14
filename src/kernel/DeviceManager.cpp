//
// Created by user on 9/29/15.
//
#include "../stdafx.h"
#include "DeviceManager.h"


namespace Flug {
	DeviceManager::DeviceManager() :
	Module("DeviceManager") { }

	DeviceManager::~DeviceManager() { }

	bool DeviceManager::initModule() {
		return true;
	}

	bool DeviceManager::destroyModule() {
		return true;
	}

	bool DeviceManager::handleRequest(Request & req, Response & resp) {
		std::string reqtype = req.m_json["reqtype"].asString();
		if (reqtype == "getDevicesList") {
			return handleGetDevicesList(req, resp);
		} else if (reqtype == "getTypesList") {
			return handleGetDeviceTypesList(req, resp);
		} else if (reqtype == "getDevicesOfType") {
			return handleGetDevicesOfType(req, resp);
		}
		return false;
	}

	bool DeviceManager::initDevices(Dispatcher * dispatcher, Json::Value &devList) {

		//TODO: Da faq.


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

	bool DeviceManager::handleGetDeviceTypesList(Request &req, Response &resp) {
		Json::Value root;
		root["status"] = "success";
		std::list<std::string> types;
		for (auto iter = m_devices.begin(); iter != m_devices.end(); iter++) {
			std::string devType = iter->second->getDeviceType();
			if (std::find(types.begin(), types.end(), devType) == types.end()) {
				types.push_back(devType);
			}
		}

		int i = 0;
		for (auto iter = types.begin(); iter != types.end(); iter++) {
			root["types"][i++] = *iter;
		}

		resp = root;
		return true;
	}

	bool DeviceManager::handleGetDevicesOfType(Request &req, Response &resp) {
		Json::Value root;
		std::string devType = req.m_json["devtype"].asString();
		root["type"] = devType;
		root["status"] = "success";
		int i = 0;
		for (auto iter = m_devices.begin(); iter != m_devices.end(); iter++) {
			if (devType == iter->second->getDeviceType()) {
				root["devices"][i++] = iter->first;
			}
		}

		resp = root;
		return true;
	}

	bool DeviceManager::handleGetDevicesList(Request &req, Response &resp) {
		Json::Value root;
		root["status"] = "success";
		int i = 0;
		std::cout << "Device enumeration" << std::endl;

		std::map<std::string, DeviceDriver*>::iterator iter;
		for (iter = m_devices.begin();
			 iter != m_devices.end(); iter++, i++) {
			root["devices_list"][i]["name"] = iter->first;
			root["devices_list"][i]["type"] = iter->second->getDeviceType();
			std::cout  << "Device :: "<< iter->first << std::endl;
		}

		std::cout << "Ended device enumeration" << std::endl;
		resp = root;
		return true;
	}
}