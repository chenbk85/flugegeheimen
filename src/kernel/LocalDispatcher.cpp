//
// Created by user on 02.02.16.
//

#include "../stdafx.h"
#include "LocalDispatcher.h"

namespace  Flug {
    LocalDispatcher::LocalDispatcher() { }
    LocalDispatcher::~LocalDispatcher() { }
    bool LocalDispatcher::checkForResponses(Response &response) {
        for (auto iter = m_modules.begin(); iter != m_modules.end(); iter++) {
            if (iter->second.m_module) {
                Response tempResp;
                if (iter->second.m_module->popLocalResponse(tempResp)) {
                    response = tempResp;
                    return true;
                }
            }
        }
        return false;
    }

    bool LocalDispatcher::checkForRequests(Request &request) {
        for (auto iter = m_modules.begin(); iter != m_modules.end(); iter++) {
            if (iter->second.m_module) {
                Request tempReq;
                if (iter->second.m_module->popLocalRequest(tempReq)) {
                    request = tempReq;
                    return true;
                }
            }
        }
        return false;
    }

    bool LocalDispatcher::dispatchRequest(const std::string &request, Module *sender) {
        Json::Reader reader;
        Json::Value root;
        std::string modName;
        Request req(request, sender);

        if (!reader.parse(request, root)) {
            //send error message, cancel processing
            return false;
        }
        modName = root["subsystem"].asString();

        std::cout << "Module name: " << modName << std::endl;
        if (!m_modules[modName].m_module->pushLocalRequest(req)) {
            //send error "queue overflow" or "wrong request format"
            return false;
        }
        return true;
    }

    bool LocalDispatcher::hasModule(const std::string &request) {
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

    void LocalDispatcher::registerModule(const std::string &moduleName, Module *module) {
        m_modules[moduleName] = HandlerRecord(moduleName, module);
        std::cout << "Registered LOCAL module " << moduleName << std::endl;
    }


}