//
// Created by user on 28.10.15.
//

#include "../stdafx.h"
#include "ArchiveModule.h"


namespace Flug {

    ArchiveModule::ArchiveModule(InterlockedArchiveBackend * backend)
            : Module("database"), m_backend(backend) {

    }

    ArchiveModule::~ArchiveModule() {

    }

    bool ArchiveModule::loadConfig(Json::Value &config) {
        std::cout << m_backend << " Loading database config: " << JsonBson(config).str() << std::endl;
        m_backend->setDbCredentials(config["uri"].asString(),
                                    config["database"].asString(),
                                    config["user"].asString(),
                                    config["pass"].asString()
                                    );
        return true;
    }

    bool ArchiveModule::initModule() {
        //m_backend->connect();
        return true;
    }

    bool ArchiveModule::destroyModule() {
        //m_backend->disconnect();
        return true;
    }

    bool ArchiveModule::handleRequest(Request &req, Response &resp) {
        std::string reqtype = req.m_json["reqtype"].asString();
        if (reqtype == "request") {
            return handleQuery(req, resp);
        }
        return false;
    }

    bool ArchiveModule::handleQuery(Request &req, Response &resp) {
        Json::Value root;

        JsonBson result;
        m_backend->findUnique(req.m_json["collection"].asString(), req.m_json["request"].asString(), result);
        root["data"] = result;
        root["status"] = "success";

        resp = root;
        return true;
    }

    bool ArchiveModule::rebootModule() {
        return false;
    }
}