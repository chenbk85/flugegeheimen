//
// Created by user on 9/29/15.
//

#include "../stdafx.h"
#include "Dispatcher.h"
#include "RequestEnumerator.h"


namespace Flug {
	Dispatcher::Dispatcher() {
	}

	Dispatcher::~Dispatcher() {
	}

	void Dispatcher::registerModule(const std::string &moduleName, Module *module) {
		m_modules[moduleName] = HandlerRecord(moduleName, module);
	}

	bool Dispatcher::dispatchRequest(const std::string &request, PollingBuffer * pbuf) {
		Json::Reader reader;
		Json::Value root;
		std::string modName;
		Request * req = new Request(request, pbuf);

		if (!reader.parse(request, root)) {
			//send error message, cancel processing
			return false;
		}
		modName = root["subsystem"].asString();
		std::cout << "Module name: " << modName << std::endl;
		if (!m_modules[modName].m_module->pushRequest(*req)) {
			//send error "queue overflow" or "wrong request format"
			return false;
		}
		return false;
	}


	bool Dispatcher::checkForResponses(Response &response) {
		for (auto iter = m_modules.begin(); iter != m_modules.end(); iter++) {
			if (iter->second.m_module) {
				Response tempResp;
				if(iter->second.m_module->popResponse(tempResp)) {
					response = tempResp;
					return true;
				}
			}
		}
		return false;
	}
}