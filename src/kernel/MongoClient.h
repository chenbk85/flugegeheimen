//
// Created by user on 30.10.15.
//

#ifndef PROJECT_MONGOCLIENT_H
#define PROJECT_MONGOCLIENT_H

#include "JsonBson.h"

namespace Flug {
    class MongoClient {
    public:
        MongoClient();

        virtual ~MongoClient();

        void connect(const std::string &addr, const std::string db, const std::string user, const std::string &pass);

        void disconnect();

        void updateCollectionsList();

        bool hasCollection(const std::string &collection);

        void addCollection(const std::string &collection);

        void insert(const std::string &collection, const JsonBson &document);

        void updateUnique(const std::string &collection, const JsonBson &search, const JsonBson &update,
                          bool upsert = true);

        void findUnique(const std::string &collection, const JsonBson &search, JsonBson &result);

        void find(const std::string &collection, const JsonBson &search, std::list<JsonBson> &results);

    protected:

        mongo::DBClientConnection m_connection;
        std::list<std::string> m_collections;
        std::string m_db;

    };
}


#endif //PROJECT_MONGOCLIENT_H
