//
// Created by user on 9/29/15.
//

#include "../stdafx.h"
#include "Module.h"
#include "LocalDispatcher.h"

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

        while (m_localInQueue.pop(request)) {
            m_inQueueCounter--;
            try {
                if (!handleRequestWraper(request, response)) {
                    root["status"] = "error";
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
            m_localOutQueue.push(response);
        }

        while (m_remoteInQueue.pop(request)) {
            m_inQueueCounter--;
            try {
                if (!handleRequestWraper(request, response)) {
                    root["status"] = "error";
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
            m_remoteOutQueue.push(response);
        }
    }

    bool Module::pushRemoteRequest(Request &req) {
        bool ret = m_remoteInQueue.push(req);
        if (ret) {
            m_inQueueCounter++;
            m_inQueueCv.notify_one();
        }
        return ret;
    }

    bool Module::popRemoteResponse(Response &resp) {
        return m_remoteOutQueue.pop(resp);
    }

    bool Module::handleRequestWraper(Request &req, Response &resp) {
        if (!req.m_parsed) {
            throw std::runtime_error("Non-parsed request sent to module handling proc");
        }

        resp.m_id = req.m_id;
        resp.m_pbuf = req.m_pbuf;
        resp.m_local = req.m_local;
        resp.m_module = req.m_module;

        bool ret = handleRequest(req, resp);

        resp.m_id = req.m_id;
        resp.m_pbuf = req.m_pbuf;
        resp.m_local = req.m_local;
        resp.m_module = req.m_module;
        return ret;
    }

    bool Module::pushLocalRequest(Request &req) {
        bool ret = m_localInQueue.push(req);
        if (ret) {
            m_inQueueCounter++;
            m_inQueueCv.notify_one();
        }
        return ret;
    }

    bool Module::popLocalResponse(Response &resp) {
        return m_localOutQueue.pop(resp);
    }

    void Module::localRequest(Request &req, Response &resp) {
        //Send request
        m_localRequestsQueue.push(req);
        //Get response
        m_locRespCounter.store(0);
        std::unique_lock<std::mutex> lk(m_locRespMutex);
        m_locRespCv.wait(lk, [this]{return m_locRespCounter.load() > 0;});
        lk.unlock();
        if(!m_localResponsesQueue.pop(resp)) {
            throw std::runtime_error("Skew in local responses handling logic");
        }
    }

    void Module::localMultiRequest(std::vector<Request> &reqs,
                                   std::vector<Response> &resps) {
        size_t count = reqs.size();
        for (auto req: reqs) {
            m_localRequestsQueue.push(req);
        }
        m_locRespCounter.store(0);
        std::unique_lock<std::mutex> lk(m_locRespMutex);
        m_locRespCv.wait(lk, [this, count]{return m_locRespCounter.load() == count;});
        lk.unlock();

        std::vector<Response> tmpResps;
        Response tmpRes;
        for (size_t i = 0; i < count; i++) {
            if (!m_localResponsesQueue.pop(tmpRes)) {
                throw std::runtime_error("Skew in local responses handling logic");
            }
            tmpResps.push_back(tmpRes);
        }

        resps.clear();
        for (auto req: reqs) {
            for (auto resp: tmpResps) {
                if (resp.m_id == req.m_id) {
                    resps.push_back(resp);
                    break;
                }
            }
        }
    }

    bool Module::pushLocalResponse(Response &resp) {
        bool ret = m_localResponsesQueue.push(resp);
        if (ret) {
            m_locRespCounter++;
            m_locRespCv.notify_one();
        } else {
            throw std::runtime_error("Failed to submit local response");
        }
        return ret;
    }


    bool Module::popLocalRequest(Request &req) {
        return m_localRequestsQueue.pop(req);
    }
}