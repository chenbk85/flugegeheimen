//
// Created by user on 10/3/15.
//

#pragma once

#include "RequestEnumerator.h"
#include "PollingBuffer.h"


namespace Flug {
    class Module;

    /**
     * Request structure contains the requests to module passed by other
     * modules and/or user interfaces
     */
    struct Request {
        Request() { };

        virtual ~Request() { };

        Request(const std::string &request, PollingBuffer *pbuf) {
            Json::Reader parser;
            m_string = request;
            if (parser.parse(request, m_json)) {
                m_parsed = true;
            } else {
                m_parsed = false;
            }
            RequestEnumerator &enumer = RequestEnumerator::getInstance();
            m_id = enumer.getRequestId();
            m_pbuf = pbuf;
            m_specificSender = false;
            m_local = false;
        };

        Request(const std::string &request, Module *module) {
            Json::Reader parser;
            m_string = request;
            if (parser.parse(request, m_json)) {
                m_parsed = true;
            } else {
                m_parsed = false;
            }
            RequestEnumerator &enumer = RequestEnumerator::getInstance();
            m_id = enumer.getRequestId();
            m_module = module;
            m_specificSender = false;
            m_local = true;
        };

        Request(const Request &oth) :
                m_id(oth.m_id), m_json(oth.m_json), m_string(oth.m_string),
                m_parsed(oth.m_parsed), m_pbuf(oth.m_pbuf),
                m_specificSender(oth.m_specificSender), m_sender(oth.m_sender),
                m_local(oth.m_local), m_module(oth.m_module) {
        };

        void updateString() {
            Json::FastWriter writer;
            m_string = writer.write(m_json);
        };

        Request &operator=(const Request &oth) {
            m_id = oth.m_id;
            m_parsed = oth.m_parsed;
            m_string = oth.m_string;
            m_json = oth.m_json;
            m_pbuf = oth.m_pbuf;
            m_local = oth.m_local;
            m_module = oth.m_module;
        };

        bool m_local; // Is it a local request
        std::string m_string;
        Json::Value m_json;
        bool m_parsed;
        uint64_t m_id;
        bool m_specificSender;
        std::string m_sender;
        PollingBuffer *m_pbuf; //In case of remote request
        Module *m_module; //In case of local reqest
    };

}
