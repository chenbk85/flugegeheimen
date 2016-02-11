//
// Created by user on 28.10.15.
//

#include "../stdafx.h"
#include "ArchiveModule.h"

namespace Flug {

    ArchiveModule::ArchiveModule(ArchiveBackend * backend)
            : Module("database"), m_backend(backend) {

    }

    ArchiveModule::~ArchiveModule() {

    }

    bool ArchiveModule::loadConfig(Json::Value &config) {
        std::cout << m_backend << " Loading database config: " <<
                JsonBson(config).str() << std::endl;
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
        if (reqtype == "findUnique") {
            return handleFindUnique(req, resp);
        } else if (reqtype == "getDatastoresList") {
            return handleGetDatastoresList(req, resp);
        } else if (reqtype == "registerDatastore") {
            return handleRegisterDatastore(req, resp);
        } else if (reqtype == "insert") {
            return handleInsert(req, resp);
        }
        return false;
    }

    bool ArchiveModule::handleFindUnique(Request &req, Response &resp) {
        Json::Value root;

        JsonBson result;
        m_backend->findUnique(req.m_json["collection"].asString(),
                              req.m_json["request"].asString(), result);
        root["data"] = result;
        root["status"] = "success";

        resp = root;
        return true;
    }

    bool ArchiveModule::rebootModule() {
        return false;
    }

    bool ArchiveModule::handleGetDatastoresList(Request &req, Response &resp) {
        Json::Value root;

        JsonBson result;
        m_backend->findUnique("dbinfo",
                              JsonBson(std::string(
                                      "{\"dbInfoSection\":\"datastoresList\"}")),
                              result);
        root["data"] = result;
        root["status"] = "success";

        resp = root;
        return true;
    }

    bool ArchiveModule::handleRegisterDatastore(Request &req, Response &resp) {
        Json::Value root;
        std::string dsName = req.m_json["name"].asString();

        Json::Value search, update;
        search["dbInfoSection"] = "datastoresList";
        update["$push"]["datastores"]["name"] = dsName;
        m_backend->updateUnique("dbinfo", search, update, true);
        m_backend->addCollection(getDatastoreName(dsName));

        root["status"] = "success";
        resp = root;
        return true;
    }

    std::string ArchiveModule::getDatastoreName(const std::string &name) {
        return "datastore_" + name;
    }

    bool ArchiveModule::handleInsert(Request &req, Response &resp) {
        Json::Value root;
        std::string collection = req.m_json["datastore"].asString();

        m_backend->insert(collection, req.m_json["data"]);

        root["status"] = "success";
        resp = root;
        return true;
    }
}