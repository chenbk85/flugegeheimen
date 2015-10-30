//
// Created by user on 30.10.15.
//

#include "../stdafx.h"
#include "InterlockedArchiveBackend.h"


#define DB_COLL_DBINFO "dbinfo"
#define DB_INFO_SECTION "dbInfoSection"
#define DB_INFO_DEVICES_LIST "devicesList"

#define ARCH_LOCK_MACRO int asdfaskjdhflasd = 9 //std::lock_guard<std::mutex> __lk(m_lock)

namespace Flug {



    InterlockedArchiveBackend::InterlockedArchiveBackend() {
    }

    InterlockedArchiveBackend::~InterlockedArchiveBackend() {
    }

    void InterlockedArchiveBackend::setupCollections() {
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
    }

    bool InterlockedArchiveBackend::checkDeviceHasArchive(const std::string &device, const std::string &devtype) {
        updateArchiveDevicesList();
        for (auto dev: m_deviceArchives) {
            if (dev.m_name == device && dev.m_type == devtype) {
                return true;
            }
        }
        return false;
    }

    bool InterlockedArchiveBackend::updateArchiveDevicesList() {
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

    std::string InterlockedArchiveBackend::getDeviceArchName(const std::string &device, const std::string &devType) {
        return std::string("devarch_" + devType + "_" + device);
    }

    void InterlockedArchiveBackend::addDeviceArchive(const std::string &device, const std::string &devtype) {
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


    void InterlockedArchiveBackend::connect() {
        ARCH_LOCK_MACRO;
        std::cout << "Connecting to database" <<
                " uri: " << m_mongoUri <<
                " dbname: " << m_dbname <<
                " user: " << m_user <<
                " pass: " << m_pass << std::endl;
        m_client.connect(m_mongoUri, m_dbname, m_user, m_pass);
        setupCollections();
    }

    void InterlockedArchiveBackend::updateCollectionsList() {
        ARCH_LOCK_MACRO;
        m_client.updateCollectionsList();
    }

    bool InterlockedArchiveBackend::hasCollection(const std::string &collection) {
        ARCH_LOCK_MACRO;
        return m_client.hasCollection(collection);
    }

    void InterlockedArchiveBackend::addCollection(const std::string &collection) {
        ARCH_LOCK_MACRO;
        m_client.addCollection(collection);
    }

    void InterlockedArchiveBackend::insert(const std::string &collection, const JsonBson &document) {
        ARCH_LOCK_MACRO;
        m_client.insert(collection, document);
    }


    void InterlockedArchiveBackend::updateUnique(const std::string &collection, const JsonBson &search,
                                                 const JsonBson &update, bool upsert) {
        ARCH_LOCK_MACRO;
        m_client.updateUnique(collection, search, update, upsert);

    }

    void InterlockedArchiveBackend::findUnique(const std::string &collection, const JsonBson &search,
                                               JsonBson &result) {
        ARCH_LOCK_MACRO;
        m_client.findUnique(collection, search, result);

    }

    void InterlockedArchiveBackend::disconnect() {
        ARCH_LOCK_MACRO;
        m_client.disconnect();
    }

    void InterlockedArchiveBackend::setDbCredentials(const std::string &addr, const std::string &db,
                                                     const std::string &user, const std::string &pass) {

        ARCH_LOCK_MACRO;
        m_mongoUri = addr;
        m_dbname = db;
        m_user = user;
        m_pass = pass;

    }
}

