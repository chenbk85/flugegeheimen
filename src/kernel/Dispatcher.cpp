//
// Created by user on 9/29/15.
//

#include "../stdafx.h"
#include "Dispatcher.h"


namespace Flug {
	Dispatcher::Dispatcher() {
	}

	Dispatcher::~Dispatcher() {
	}

	void Dispatcher::registerModule(const std::string &moduleName, Module *module) {
		m_modules[moduleName] = HandlerRecord(moduleName, module);
	}

	bool Dispatcher::dispatchRequest(const std::string &request) {
		Json::Reader reader;
		Json::Value root;
		std::string modName;

		if (!reader.parse(request, root)) {
			//send error message, cancel processing
			return false;
		}
		modName = root["submodule"].asString();
		if (!m_modules[modName].m_module->pushRequest(request)) {
			//send error "queue overflow" or "wrong request format"
			return false;
		}
		return false;
	}

	bool Dispatcher::checkForResponses(std::string &response) {
		for (auto iter = m_modules.begin(); iter != m_modules.end(); iter++) {
			if (iter->second.m_module) {
				if(iter->second.m_module->popResponse(response)) {
					return true;
				}
			}
		}
		return false;
	}
}