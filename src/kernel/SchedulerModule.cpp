//
// Created by user on 09.02.16.
//

#include "../stdafx.h"
#include "JsonBson.h"
#include "SchedulerModule.h"

namespace Flug {

    SchedulerModule::SchedulerModule(const std::string &configName) :
            Module(configName) {

    }

    SchedulerModule::~SchedulerModule() {

    }

    bool SchedulerModule::initModule() {
        return true;
    }

    bool SchedulerModule::destroyModule() {
        return true;
    }

    bool SchedulerModule::handleRequest(Request &req, Response &resp) {
        return false;
    }

    void SchedulerModule::registerLang(BaseScriptBuilder *script, const std::string &name) {
        if (m_langs.find(name) != m_langs.end()) {
            throw std::runtime_error(("Script is already registered: " + name).c_str());
        }
        m_langs[name] = script;
    }

    void SchedulerModule::killTask(const std::string &name) {
        m_tasks[name]->destroyModule();
        delete m_tasks[name];
        m_tasks.erase(name);
    }

    bool SchedulerModule::loadConfig(Json::Value &config) {
        std::cout << "Loading script configuration" << std::endl <<
                JsonBson(config).str() << std::endl;

        for (int i = 0; i < config.size(); i++) {
            Json::Value cfg = config[i];
            startTask(cfg["lang"].asString(), cfg["name"].asString(),
                    cfg["script"].asString());
        }

        return true;
    }

    void SchedulerModule::startTask(const std::string &lang, const std::string &name,
                                    const std::string & script) {
        if (m_langs.find(lang) == m_langs.end()) {
            throw std::runtime_error(("Can't find fucking lang engine: " + lang).c_str());
        }
        ScriptModule *task = m_langs[lang]->buildModule(name);
        task->loadScript(script);
        task->initModule();
        registerTask(task, name);
    }

    void SchedulerModule::registerTask(ScriptModule *task, const std::string &name) {
        m_tasks[name] = task;
    }

    bool SchedulerModule::rebootModule() {
        return false;
    }

    void SchedulerModule::initTasks() {
        for (auto task: m_tasks) {
            task.second->initModule();
        }
    }
}
