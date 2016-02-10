//
// Created by user on 02.02.16.
//

#pragma once

#include "../stdafx.h"
#include "Module.h"
#include "Response.h"

namespace Flug {
    class LocalDispatcher {
        struct HandlerRecord {
            HandlerRecord(const std::string &name, Module *module) :
                    m_name(name), m_module(module) { };

            HandlerRecord() :
                    m_name("NULLMODULE"), m_module(nullptr) { };

            HandlerRecord &operator=(const HandlerRecord &oth) {
                m_name = oth.m_name;
                m_module = oth.m_module;
                return *this;
            };

            HandlerRecord(const HandlerRecord &oth) :
                    m_name(oth.m_name), m_module(oth.m_module) {
            };

            std::string m_name;
            Module *m_module;
        };

    public:
        LocalDispatcher ();

        virtual ~LocalDispatcher();

        void registerModule(const std::string &moduleName, Module *module);

        bool dispatchRequest(Request &request);

        bool checkForResponses(Response &response);

        bool checkForRequests(Request & request);

        bool hasModule(const std::string &request);

        Module * getModule (const std::string & name);

    protected:
    private:
        std::map<std::string, HandlerRecord> m_modules;
    };

    class LocalDispatcherPtr {
    private:
        LocalDispatcher * m_ptr;
        LocalDispatcherPtr() {};
    public:
        LocalDispatcher * getPointer () {
            return m_ptr;
        };
        void setPointer (LocalDispatcher * ptr) {
            m_ptr = ptr;
        };
        static LocalDispatcherPtr & get () {
            static LocalDispatcherPtr _instance;
            return _instance;
        };
    };
}

