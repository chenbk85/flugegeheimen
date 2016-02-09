//
// Created by user on 09.02.16.
//

#pragma once

#include "Module.h"
#include "ScriptModule.h"

namespace Flug {

    class SchedulerModule : public Module {
    private:
        class BaseScriptBuilder {
        public:
            virtual ScriptModule *buildModule(const std::string &name) = 0;
        };

        template<class S>
        class ScriptBuilder : public BaseScriptBuilder {
            ScriptModule *buildModule(const std::string &name) {
                return new S(name);
            }
        };

    public:
        struct Rule {
            enum Type {
                SC_TIME, SC_PERIODIC
            };

            Type m_type;
            size_t m_tick;

            Rule(Type type, size_t tick) { };
        };

        SchedulerModule() = delete;

        SchedulerModule(const std::string &configName);

        virtual ~SchedulerModule();

        virtual bool initModule();

        virtual bool destroyModule();

        virtual bool handleRequest(Request &req, Response &resp);

        virtual bool loadConfig(Json::Value &config);

        virtual bool rebootModule();

        ScriptModule * startTask(const std::string &lang, const std::string &name,
                       const std::string & script);

        template<class T>
        void addLang(const std::string &name) {
            BaseScriptBuilder *script = new ScriptBuilder<T>();
            registerLang(script, name);
        };

        void registerLang(BaseScriptBuilder *script, const std::string &name);

        void registerTask(ScriptModule *task, const std::string &name);

        void initTasks ();

        void killTask(const std::string &name);

    protected:
    private:


        std::map<std::string, BaseScriptBuilder *> m_langs;
        std::map<std::string, ScriptModule *> m_tasks;
    };

}

