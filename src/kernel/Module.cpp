//
// Created by user on 9/29/15.
//

#include "../stdafx.h"
#include "Module.h"

#define MAX_MODULE_QUEUE_DEPTH 100
#define MODULE_REQUEST_THROTTLING 10

namespace Flug {
    Module::Module(const std::string &configName) :
            m_quitState(false), Configurable(configName) {
        m_inQueueCounter.store(0);
    }

    void Module::start() {
        m_thread = new std::thread(&Module::moduleProc, this);
    }

    Module::~Module() { }

    void Module::moduleProc() {
        try {
            if (!this->initModule()) {
                std::cout << "Failed to init module " << getConfigTag() << std::endl;
            }
        } catch (std::exception &ex) {
            std::cout << "Failed to init module " << getConfigTag() << ": exception dropped:" << std::endl <<
            ex.what() << std::endl;
        }
        while (!this->m_quitState) {
            this->mainLoop();
        }
        try {
            if (!this->destroyModule()) {
                std::cout << "Failed to destroy module " << getConfigTag() << std::endl;
            }
        } catch (std::exception &ex) {
            std::cout << "Failed to init module " << getConfigTag() << ": exception dropped:" << std::endl <<
            ex.what() << std::endl;
        }
    }

    void Module::mainLoop() {
        std::unique_lock<std::mutex> lk(m_inQueueMutex);
        m_inQueueCv.wait(lk, [this]{return m_inQueueCounter.load() > 0;});
        lk.unlock();
        handleIncomingRequests();
        //std::this_thread::sleep_for(
        //        std::chrono::milliseconds(MODULE_REQUEST_THROTTLING));
    }


    void Module::handleIncomingRequests() {
        Request request;
        Response response;
        Json::Value root;
        while (m_inQueue.pop(request)) {
            m_inQueueCounter--;
            try {
                if (!handleRequestWraper(request, response)) {
                    root["staus"] = "error";
                    root["description"] = "Request handler returned false";
                    response = root;
                    std::cout << response.m_string << std::endl;
                }
            } catch (std::exception & ex) {
                root["status"] = "error";
                root["description"] = "Exception dropped: " + std::string (ex.what());
                response = root;
                std::cout << response.m_string << std::endl;
            }
            m_outQueue.push(response);
        }
    }

    bool Module::pushRequest(Request &req) {
        bool ret = m_inQueue.push(req);
        if (ret) {
            m_inQueueCounter++;
            m_inQueueCv.notify_one();
        }
        return ret;
    }

    bool Module::popResponse(Response &resp) {
        return m_outQueue.pop(resp);
    }

    bool Module::handleRequestWraper(Request &req, Response &resp) {
        if (!req.m_parsed) {
            throw std::runtime_error("Non-parsed request sent to module handling proc");
        }

        resp.m_id = req.m_id;
        resp.m_pbuf = req.m_pbuf;

        return handleRequest(req, resp);
    }

}