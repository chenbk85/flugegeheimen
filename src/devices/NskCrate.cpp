//
// Created by user on 09.11.15.
//

#include "../stdafx.h"
#include "../kernel/MulticastBuilder.h"
#include "NskCrate.h"

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
        } else if (reqtype == "setPagesCount") {
            return handleSetPagesCount(req, resp);
        } else if (reqtype == "waitForTrigger") {
            return handleWaitForTrigger(req, resp);
        } else if (reqtype == "softStart") {
            return handleSoftStart(req, resp);
        } else if (reqtype == "calibrate") {
            return handleCalibration(req, resp);
        } else if (reqtype == "downloadData") {
            return handleDownloadData(req, resp);
        } else if (reqtype == "setOffset") {
            return handleSetOffset (req, resp);
        } else if (reqtype == "ping") {
            return handlePing(req, resp);
        } else if (reqtype == "getAdcsList") {
            return handleGetAdcsList(req, resp);
        } else if (reqtype == "reconnect") {
            return handleReconnect(req, resp);
        } else if (reqtype == "loadFirmware") {
            return handleLoadFirmware(req, resp);
        }
        return false;
    }

    bool NskCrate::handleGetData(Request &req, Response &resp) {
        Json::Value root;
        MulticastBuilder mcast(m_adcs, "getData", this);
        localMultiRequest(mcast.getReqs(), mcast.getResps());

        int i = 0;
        for (auto lresp: mcast.getResps()) {
            root["data"][i] = (Json::Value)(JsonBson(lresp.m_string));
            i++;
        }

        root["status"] = "success";
        root["xincr"] = 1;
        resp = root;
        return true;
    }

    bool NskCrate::initModule() {
        return true;
    }

    bool NskCrate::destroyModule() {
        return true;
    }

    bool NskCrate::handleSetPagesCount(Request &req, Response &resp) {
        Json::Value root;
        MulticastBuilder mcast(m_adcs, "setPagesCount", this);
        mcast.addField("pagesCount", req.m_json["pagesCount"].asUInt());
        localMultiRequest(mcast.getReqs(), mcast.getResps());
        if (mcast.getStatus()) {
            root["status"] = "success";
            resp = root;
            return true;
        } else {
            root["status"] = "failure";
            resp = root;
            return false;
        }
    }

    bool NskCrate::handleSoftStart(Request &req, Response &resp) {
        Json::Value root;
        MulticastBuilder mcast(m_adcs, "softStart", this);
        mcast.addField("pagesCount", (int) req.m_json["pagesCount"].asInt());
        localMultiRequest(mcast.getReqs(), mcast.getResps());
        if (mcast.getStatus()) {
            root["status"] = "success";
            resp = root;
            return true;
        } else {
            root["status"] = "failure";
            resp = root;
            return false;
        }
    }

    bool NskCrate::handleWaitForTrigger(Request &req, Response &resp) {
        Json::Value root;
        MulticastBuilder mcast(m_adcs, "waitForTrigger", this);
        localMultiRequest(mcast.getReqs(), mcast.getResps());
        if (mcast.getStatus()) {
            root["status"] = "success";
            resp = root;
            return true;
        } else {
            root["status"] = "failure";
            resp = root;
            return false;
        }
    }

    bool NskCrate::handleCalibration(Request &req, Response &resp) {
        Json::Value root;
        MulticastBuilder mcast(m_adcs, "calibrate", this);
        localMultiRequest(mcast.getReqs(), mcast.getResps());
        if (mcast.getStatus()) {
            root["status"] = "success";
            resp = root;
            return true;
        } else {
            root["status"] = "failure";
            resp = root;
            return false;
        }
    }

    bool NskCrate::handleDownloadData(Request &req, Response &resp) {
        Json::Value root;
        MulticastBuilder mcast(m_adcs, "downloadData", this);
        localMultiRequest(mcast.getReqs(), mcast.getResps());
        if (!mcast.getStatus()) {
            root["status"] = "error";
            root["description"] = "Failed to download from one or more module(s). Please, check connection.";
            resp = root;
            return true;
        }

        int i = 0;
        for (auto lresp: mcast.getResps()) {
            root["data"][i] = lresp.m_json["data"][0];
            i++;
        }

        i = 0;
        for (auto adc: m_adcs) {
            root["adcs"][i++] = adc;
        }


        root["status"] = "success";
        root["xincr"] = 1;
        resp = root;
        return true;
    }

    bool NskCrate::handleSetOffset(Request &req, Response &resp) {
        Json::Value root;
        MulticastBuilder mcast(m_adcs, "calibrate", this);
        mcast.addField("offset", req.m_json["offset"].asUInt());
        localMultiRequest(mcast.getReqs(), mcast.getResps());
        root["status"] = "success";
        resp = root;
        return true;
    }

    bool NskCrate::handlePing(Request &req, Response &resp) {
        Json::Value root;
        MulticastBuilder mcast(m_adcs, "ping", this);
        localMultiRequest(mcast.getReqs(), mcast.getResps());

        int i = 0;
        for (auto lresp: mcast.getResps()) {
            root["data"][i] = lresp.m_json["data"];
            i++;
        }

        root["status"] = "success";
        resp = root;
        return true;
    }

    bool NskCrate::handleGetAdcsList(Request &req, Response &resp) {
        Json::Value root;

        int i = 0;
        for (auto adc: m_adcs) {
            root["adcs"][i] = adc;
            i++;
        }

        root["status"] = "success";
        resp = root;
        return true;
    }

    bool NskCrate::handleReconnect(Request &req, Response &resp) {
        Json::Value root;
        MulticastBuilder mcast(m_adcs, "reconnect", this);
        localMultiRequest(mcast.getReqs(), mcast.getResps());
        if (mcast.getStatus()) {
            root["status"] = "success";
            resp = root;
            return true;
        } else {
            root["status"] = "failure";
            resp = root;
            return false;
        }
    }

    bool NskCrate::handleLoadFirmware(Request &req, Response &resp) {
        Json::Value root;
        MulticastBuilder mcast(m_adcs, "loadFirmware", this);
        localMultiRequest(mcast.getReqs(), mcast.getResps());
        if (mcast.getStatus()) {
            root["status"] = "success";
            resp = root;
            return true;
        } else {
            root["status"] = "failure";
            resp = root;
            return false;
        }
    }
}

