//
// Created by user on 9/29/15.
//

#include "../stdafx.h"
#include "RemoteDispatcher.h"
#include "RequestEnumerator.h"


namespace Flug {
    RemoteDispatcher::RemoteDispatcher() {
    }

    RemoteDispatcher::~RemoteDispatcher() {
    }

    void RemoteDispatcher::registerModule(const std::string &moduleName, Module *module) {
        m_modules[moduleName] = HandlerRecord(moduleName, module);
        std::cout << "Registered REMOTE module " << moduleName << std::endl;
    }

    bool RemoteDispatcher::dispatchRequest(const std::string &request, PollingBuffer *pbuf) {
        Json::Reader reader;
        Json::Value root;
        std::string modName;
        Request req(request, pbuf);

        if (!reader.parse(request, root)) {
            //send error message, cancel processing
            return false;
        }
        modName = root["subsystem"].asString();

        std::cout << "Module name: " << modName << std::endl;
        if (!m_modules[modName].m_module->pushRemoteRequest(req)) {
            //send error "queue overflow" or "wrong request format"
            return false;
        }
        return true;
    }


    bool RemoteDispatcher::checkForResponses(Response &response) {
        for (auto iter = m_modules.begin(); iter != m_modules.end(); iter++) {
            if (iter->second.m_module) {
                Response tempResp;
                if (iter->second.m_module->popRemoteResponse(tempResp)) {
                    response = tempResp;
                    return true;
                }
            }
        }
        return false;
    }

    bool RemoteDispatcher::hasModule(const std::string &request) {
        Json::Reader reader;
        Json::Value root;
        std::string modName;


        if (!reader.parse(request, root)) {
            return false;
        }

        modName = root["subsystem"].asString();

        if (m_modules.find(modName) == m_modules.end()) {
            return false;
        }
        return true;
    }
}

