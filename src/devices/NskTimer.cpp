//
// Created by user on 14.10.15.
//

#include "../stdafx.h"
#include "NskTimer.h"


namespace Flug {

    NskTimer::NskTimer(const std::string &deviceInstanceName, const std::string &devType) :
    DeviceDriver(deviceInstanceName, devType) {
        m_connected = false;
    }

    NskTimer::~NskTimer() {
    }


    bool NskTimer::loadConfig(Json::Value &config) {
        m_addr = config["addr"].asString();
        m_port = config["port"].asString();
        m_channelsNumber = config["channelsNumber"].asUInt();
        m_delays.resize(m_channelsNumber);
        for (int i = 0; i < m_channelsNumber; i++) {
            m_delays[i] = config["delays"][i].asUInt();
        }
        return true;
    }

    bool NskTimer::initModule() {
        m_sock.connect(m_addr, m_port);
        m_connected = true;
    }

    bool NskTimer::destroyModule() {
        m_sock.disconnect();
    }

    bool NskTimer::handleRequest(Flug::Request &req, Flug::Response &resp) {
        std::string reqtype = req.m_json["reqtype"].asString();
        if (reqtype == "getDeviceInfo") {
            return handleIsOnline (req, resp);
        } else if (reqtype == "getChannelsDelays") {
            return handleGetChannelsDelays(req, resp);
        }
        return false;
    }

    bool NskTimer::handleIsOnline(Flug::Request &req, Flug::Response &resp) {
        Json::Value root;
        root["status"] = "success";
        if (m_connected) {
            root["deviceStatus"] = "online";
            root["deviceName"] = m_deviceName;
            root["deviceType"] = m_deviceType;
        } else {
            root["deviceStatus"] = "offline";
        }
        resp = root;
        return true;
    }

    bool NskTimer::handleSetChannelDelay(Flug::Request &req, Flug::Response &resp) {
        return false;
    }

    bool NskTimer::handleGetChannelsDelays(Flug::Request &req, Flug::Response &resp) {
        Json::Value root;
        root["status"] = "success";
        for (int i = 0; i < m_delays.size(); i++) {
            root["delays"][i] = m_delays[i];
        }
        resp = root;
        return true;
    }
}