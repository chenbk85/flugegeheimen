//
// Created by user on 03.02.16.
//

#include "../kernel/JsonBson.h"
#include "DummyCrate.h"

Flug::DummyCrate::DummyCrate(const std::string &devName, const std::string &devType) :
DeviceDriver(devName, devType){

}

Flug::DummyCrate::~DummyCrate() {

}

bool Flug::DummyCrate::initModule() {
    return true;
}

bool Flug::DummyCrate::destroyModule() {
    return true;
}

bool Flug::DummyCrate::handleRequest(Flug::Request &req, Flug::Response &resp) {
    std::string reqtype = req.m_json["reqtype"].asString();

    if (reqtype == "getData") {
        Json::Value root;
        root["status"] = "success";
        int i = 0;
        for (auto dev: m_devs) {
            Response lresp;
            Json::Value reqRoot;
            reqRoot["reqtype"] = "getDummyData";
            reqRoot["subsystem"] = dev;
            Request lreq(JsonBson(reqRoot).str(), this);
            localRequest(lreq, lresp);
            root["data"][i] = (Json::Value)(JsonBson(lresp.m_string));
            i++;
        }
        resp = root;
        return true;
    } else if (reqtype == "getDataSync") {
        Json::Value root;
        root["status"] = "success";
        std::vector<Request> reqs;
        std::vector<Response> resps;
        for (auto dev: m_devs) {
            Json::Value reqRoot;
            reqRoot["reqtype"] = "getDummyData";
            reqRoot["subsystem"] = dev;
            Request lreq(JsonBson(reqRoot).str(), this);
            reqs.push_back(lreq);
        }

        localMultiRequest(reqs, resps);

        int i = 0;
        for (auto lresp: resps) {
            root["data"][i] = (Json::Value)(JsonBson(lresp.m_string));
            i++;
        }

        resp = root;
        return true;
    }

    return false;
}

bool Flug::DummyCrate::isOnline() {
    return true;
}

bool Flug::DummyCrate::loadConfig(Json::Value &config) {
    Json::Value devicesList = config["devices"];
    for (int i = 0; i < devicesList.size(); i++) {
        m_devs.push_back(devicesList[i].asString());
    }
}
