//
// Created by user on 30.10.15.
//

#include "../stdafx.h"
#include "ArchiveBackend.h"


#define DB_COLL_DBINFO "dbinfo"
#define DB_INFO_SECTION "dbInfoSection"
#define DB_INFO_DEVICES_LIST "devicesList"
#define DB_INFO_DATASTORES_LIST "datastoresList"

namespace Flug {

    ArchiveBackend::ArchiveBackend() {
    }

    ArchiveBackend::~ArchiveBackend() {
    }

    void ArchiveBackend::setupCollections() {
        addCollection(DB_COLL_DBINFO);
        Json::Value req;
        JsonBson data;
        req[DB_INFO_SECTION] = DB_INFO_DEVICES_LIST;
        try {
            findUnique(DB_COLL_DBINFO, req, data);
        } catch (std::runtime_error & err) {
            req["devices"] = JsonBson(std::string("[]"));
            insert(DB_COLL_DBINFO, req);
        }
        req[DB_INFO_SECTION] = DB_INFO_DATASTORES_LIST;
        try {
            findUnique(DB_COLL_DBINFO, req, data);
        } catch (std::runtime_error & err) {
            req["datastores"] = JsonBson(std::string("[]"));
            insert(DB_COLL_DBINFO, req);
        }
    }

    bool ArchiveBackend::checkDeviceHasArchive(const std::string &device, const std::string &devtype) {
        updateArchiveDevicesList();
        for (auto dev: m_deviceArchives) {
            if (dev.m_name == device && dev.m_type == devtype) {
                return true;
            }
        }
        return false;
    }

    bool ArchiveBackend::updateArchiveDevicesList() {
        Json::Value req;
        req[DB_INFO_SECTION] = DB_INFO_DEVICES_LIST;

        JsonBson data;
        findUnique(DB_COLL_DBINFO, req, data);
        Json::Value jdata = data;
        for (int iter = 0;  iter < jdata["devices"].size(); iter++) {
            Json::Value &dev = jdata["devices"][iter];
            DeviceInfo devInfo;
            devInfo.m_name = dev["name"].asString();
            devInfo.m_type = dev["type"].asString();
            devInfo.m_collectionName = getDeviceArchName(devInfo.m_name, devInfo.m_type);
            m_deviceArchives.push_back(devInfo);
        }
        return true;
    }

    std::string ArchiveBackend::getDeviceArchName(const std::string &device, const std::string &devType) {
        return std::string("devarch_" + devType + "_" + device);
    }

    void ArchiveBackend::addDeviceArchive(const std::string &device, const std::string &devtype) {
        if (checkDeviceHasArchive(device, devtype)) {
            return;
        }

        Json::Value search, update;
        search[DB_INFO_SECTION] = DB_INFO_DEVICES_LIST;
        update["$push"]["devices"]["name"] = device;
        update["$push"]["devices"]["type"] = devtype;
        updateUnique(DB_COLL_DBINFO, search, update);
        addCollection(getDeviceArchName(device, devtype));

        if (!checkDeviceHasArchive(device, devtype)) {
            throw std::runtime_error("Failed to add device archive");
        }
    }


    void ArchiveBackend::connect() {
        std::cout << "Connecting to database" <<
                " uri: " << m_mongoUri <<
                " dbname: " << m_dbname <<
                " user: " << m_user <<
                " pass: " << m_pass << std::endl;
        m_client.connect(m_mongoUri, m_dbname, m_user, m_pass);
        setupCollections();
    }

    void ArchiveBackend::updateCollectionsList() {
        m_client.updateCollectionsList();
    }

    bool ArchiveBackend::hasCollection(const std::string &collection) {
        return m_client.hasCollection(collection);
    }

    void ArchiveBackend::addCollection(const std::string &collection) {
        m_client.addCollection(collection);
    }

    void ArchiveBackend::insert(const std::string &collection, const JsonBson &document) {
        m_client.insert(collection, document);
    }


    void ArchiveBackend::updateUnique(const std::string &collection, const JsonBson &search,
                                      const JsonBson &update, bool upsert) {
        m_client.updateUnique(collection, search, update, upsert);

    }

    void ArchiveBackend::findUnique(const std::string &collection, const JsonBson &search,
                                    JsonBson &result) {
        m_client.findUnique(collection, search, result);

    }

    void ArchiveBackend::disconnect() {
        m_client.disconnect();
    }

    void ArchiveBackend::setDbCredentials(const std::string &addr, const std::string &db,
                                          const std::string &user, const std::string &pass) {

        m_mongoUri = addr;
        m_dbname = db;
        m_user = user;
        m_pass = pass;

    }
}

