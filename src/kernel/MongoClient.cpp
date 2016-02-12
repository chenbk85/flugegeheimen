//
// Created by user on 30.10.15.
//

#include "../stdafx.h"
#include "MongoClient.h"

namespace Flug {

    MongoClient::MongoClient() {
        mongo::client::initialize();
    }

    MongoClient::~MongoClient() {

    }

    void MongoClient::connect(const std::string &addr, const std::string db, const std::string user,
                              const std::string &pass) {
        m_db = db;
        m_connection.connect(addr);
        std::string authErr;
        if (!m_connection.auth(db, user, pass, authErr, true)) {
            throw std::runtime_error(authErr.c_str());
        }

        updateCollectionsList();
    }

    void MongoClient::disconnect() {
    }

    void MongoClient::updateCollectionsList() {
        try {
            m_collections = m_connection.getCollectionNames(m_db);
        } catch (std::exception & ex) {
            std::cout << "There is no collections in " + m_db << std::endl;
            m_collections.clear();
        }
    }


    bool MongoClient::hasCollection(const std::string &collection) {
        updateCollectionsList();
        if (std::find(m_collections.begin(), m_collections.end(), collection) != m_collections.end()) {
            return true;
        }
        return false;
    }

    void MongoClient::addCollection(const std::string &collection) {
        if (hasCollection(collection)) {
            return;
        }

        if (!m_connection.createCollection(m_db + "." + collection)) {
            throw std::runtime_error("Failed to add (returned false) collection " + collection);
        }

        if (!hasCollection(collection)) {
            throw std::runtime_error("Failed to add collection " + collection);
        }
    }

    void MongoClient::insert(const std::string &collection, const JsonBson &document) {
        m_connection.insert(m_db + "." + collection, document);
    }

    void MongoClient::findUnique(const std::string &collection, const JsonBson &search, JsonBson &result) {
        if (!hasCollection(collection)) {
            throw std::runtime_error("No such collection " + collection);
        }

        std::shared_ptr<mongo::DBClientCursor> cursor = m_connection.query(m_db + "." + collection,
                                                                           mongo::Query(search));

        if (!cursor->more()) {
            std::stringstream ss;
            ss << cursor->itcount();
            throw std::runtime_error("There is " + ss.str() + " objects matching " + search.str() + " instead of 1");
        }

        result = cursor->next();
    }

    void MongoClient::updateUnique(const std::string &collection, const JsonBson &search, const JsonBson &update,
                                   bool upsert) {
        if (!hasCollection(collection)) {
            throw std::runtime_error("No such collection " + collection);
        }

        std::shared_ptr<mongo::DBClientCursor> cursor =
                m_connection.query(m_db + "." + collection,
                                   mongo::Query(search));

        if (cursor->itcount() != 1) {
            std::stringstream ss;
            ss << cursor->itcount();
            throw std::runtime_error("There is " + ss.str() +
                                     " objects matching " + search.str() +
                                     " instead of 1");
        }

        m_connection.update(m_db + "." + collection, mongo::Query(search), update, upsert, false);
    }

    void MongoClient::find(const std::string &collection, const JsonBson &search,
                           std::list<JsonBson> &results, const JsonBson *fields) {
        if (!hasCollection(collection)) {
            throw std::runtime_error("No such collection " + collection);
        }

        mongo::BSONObj flds = *fields;

        std::shared_ptr<mongo::DBClientCursor> cursor;
        if (fields) {
            cursor = m_connection.query(m_db + "." + collection,
                                        mongo::Query(search), 0, 0, &flds);
        } else {
            cursor = m_connection.query(m_db + "." + collection,
                                        mongo::Query(search));
        }

        while (cursor->more()) {
            results.push_back(JsonBson(cursor->next()));
        }


    }
}