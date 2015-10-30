//
// Created by user on 30.10.15.
//

#ifndef PROJECT_INTERLOCKEDARCHIVEBACKEND_H
#define PROJECT_INTERLOCKEDARCHIVEBACKEND_H

#include "JsonBson.h"
#include "MongoClient.h"

namespace Flug {
    class InterlockedArchiveBackend {
    public:

        InterlockedArchiveBackend();

        virtual ~InterlockedArchiveBackend();

        struct DeviceInfo {
            std::string m_name;
            std::string m_type;
            std::string m_collectionName;
        };


        void setDbCredentials(const std::string &addr, const std::string &db, const std::string &user,
                              const std::string &pass);

        void connect();

        void disconnect();

        void updateCollectionsList();

        bool hasCollection(const std::string &collection);

        void addCollection(const std::string &collection);

        void insert(const std::string &collection, const JsonBson &document);

        void updateUnique(const std::string &collection, const JsonBson &search, const JsonBson &update,
                          bool upsert = true);

        void findUnique(const std::string &collection, const JsonBson &search, JsonBson &result);

        void setupCollections();

        bool updateArchiveDevicesList();

        bool checkDeviceHasArchive(const std::string &device, const std::string &devtype);

        void addDeviceArchive(const std::string &device, const std::string &devtype);

        std::string getDeviceArchName(const std::string &device, const std::string &devType);

        MongoClient m_client;

        std::string m_mongoUri;
        std::string m_user;
        std::string m_pass;
        std::string m_dbname;

        std::list<DeviceInfo> m_deviceArchives;

    protected:



        std::mutex m_lock;
    };
}


#endif //PROJECT_INTERLOCKEDARCHIVEBACKEND_H
