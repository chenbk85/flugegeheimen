//
// Created by user on 09.11.15.
//

#include "../stdafx.h"
#include "NskCrate.h"
#include "../kernel/JsonBson.h"
#include "../kernel/Intercom.h"

namespace Flug {

    NskCrate::NskCrate(const std::string &devName, const std::string &devType) :
    DeviceDriver (devName, devType) {

    }

    NskCrate::~NskCrate() {

    }

    bool NskCrate::loadConfig(Json::Value &config) {
        Json::Value devicesList = config["devices"];
        for (int i = 0; i < devicesList.size(); i++) {
            m_adcs.push_back(devicesList[i].asString());
        }
    }


    bool NskCrate::handleRequest(Request &req, Response &resp) {
        std::string reqtype = req.m_json["reqtype"].asString();

        if (reqtype == "getData") {
            return handleGetData(req, resp);
        }

        return false;
    }

    bool NskCrate::handleGetData(Request &req, Response &resp) {
        Json::Value root;
        root["status"] = "success";

        for (auto adc: m_adcs) {
            Json::Value adcReq;
            adcReq["subsystem"] = adc;
            adcReq["reqtype"] = "getData";
            req.m_json = JsonBson(adcReq).str();

            Intercom::getInstance().sendRequest(adc, m_deviceName, req);
        }

        resp = root;
        return true;
    }

    bool NskCrate::initModule() {
        return true;
    }

    bool NskCrate::destroyModule() {
        return true;
    }
}