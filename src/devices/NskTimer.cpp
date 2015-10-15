//
// Created by user on 14.10.15.
//

#include "../stdafx.h"
#include "NskTimer.h"
#include "../kernel/request_builder.h"
#include "../kernel/response_parser.h"


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
        pushDelays();
    }

    bool NskTimer::destroyModule() {
        m_sock.disconnect();
    }

    bool NskTimer::handleRequest(Flug::Request &req, Flug::Response &resp) {
        std::string reqtype = req.m_json["reqtype"].asString();
        if (reqtype == "getDeviceInfo") {
            return handleIsOnline (req, resp);
        } else if (reqtype == "loadDelays") {
            return handleLoadDelays(req, resp);
        } else if (reqtype == "updateDelays") {
            return handleUpdateDelays(req, resp);
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

    bool NskTimer::handleLoadDelays(Flug::Request &req, Flug::Response &resp) {
        Json::Value root;
        root["status"] = "success";
        for (int i = 0; i < m_delays.size(); i++) {
            root["delays"][i] = m_delays[i];
        }
        resp = root;
        return true;
    }

    bool NskTimer::handleUpdateDelays(Flug::Request &req, Flug::Response &resp) {
        Json::Value root;
        root["status"] = "success";
        for (int i = 0; i < m_delays.size(); i++) {
            m_delays[i] = req.m_json["delays"][i].asUInt();
        }
        pushDelays();
        resp = root;
        return true;
    }

    void NskTimer::setChannelDelay(uint8_t chanNo, uint16_t delay) {
        Flug::RequestBuilder req;
        Flug::ResponseParser res;

        req << ((uint8_t) 0x80) << ((uint8_t) chanNo) << ((uint8_t) 0x2) << ((uint16_t) delay);
        m_sock.send(req.data(), req.size());

        char buf[4];
        uint8_t numReg, cmd;
        uint16_t val;

        m_sock.recv(buf, 4);
        res.setData(buf, 4);
        res >> numReg >> cmd >> val;
    }

    void NskTimer::pushDelays() {
        for (uint8_t chNo = 0; chNo < m_channelsNumber; chNo++) {
            setChannelDelay(chNo, m_delays[chNo]);
        }
    }

    void NskTimer::popDelays() {
        throw std::runtime_error("Not implmented");
    }
}