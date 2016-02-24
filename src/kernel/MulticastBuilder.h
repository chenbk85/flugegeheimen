//
// Created by user on 08.02.16.
//

#pragma once

#include "Request.h"
#include "Response.h"
#include "JsonBson.h"

namespace Flug {
    class MulticastBuilder {
    public:
        MulticastBuilder(const std::string &subsystem,
                         const std::string &reqtype, size_t count, Module *module) {
            for (size_t n = 0; n < count; n++) {
                Json::Value reqRoot;
                reqRoot["reqtype"] = reqtype;
                reqRoot["subsystem"] = subsystem;
                Request lreq(JsonBson(reqRoot).str(), module);
                m_requests.push_back(lreq);
            }
        };

        MulticastBuilder(const std::vector<std::string> &subsystems,
                         const std::string &reqtype, Module *module) {
            for (auto subsystem: subsystems) {
                Json::Value reqRoot;
                reqRoot["reqtype"] = reqtype;
                reqRoot["subsystem"] = subsystem;
                Request lreq(JsonBson(reqRoot).str(), module);
                m_requests.push_back(lreq);
            }
        };

        virtual ~MulticastBuilder() {};

        template<typename T>
        MulticastBuilder &addField(const std::string &name, const T &val) {
            for (int i = 0; i < m_requests.size(); i++) {
                m_requests[i].m_json[name] = val;
            }
        };

        template<typename T>
        MulticastBuilder &mapField(const std::string &name,
                                   const std::vector<T> &vals) {
            auto valPtr = vals.cbegin();
            for (auto req: m_requests) {
                req.m_json[name] = *valPtr;
                valPtr++;
                valPtr = (valPtr == vals.cend())?valPtr--:valPtr;
            }
        };

        template<typename T>
        MulticastBuilder &mapField(const std::string &name,
                                   const std::list<T> &vals) {
            auto valPtr = vals.cbegin();
            for (auto req: m_requests) {
                req.m_json[name] = *valPtr;
                valPtr++;
                valPtr = (valPtr == vals.cend())?valPtr--:valPtr;
            }
        };

        std::vector<Request> & getReqs () {
            for (auto req: m_requests) {
                req.updateString();
            }
            return m_requests;
        };

        std::vector<Response> & getResps () {
            return m_responses;
        };

        bool getStatus () {
            for (auto resp: m_responses) {
                if (resp.m_json["status"] != "success")
                    return false;
            }
            return true;
        };

    protected:
    private:
        MulticastBuilder() = delete;

        std::vector<Request> m_requests;
        std::vector<Response> m_responses;
    };
}
