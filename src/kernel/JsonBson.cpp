//
// Created by user on 29.10.15.
//

#include "../stdafx.h"
#include "JsonBson.h"

namespace Flug {
    JsonBson::JsonBson() {

    }

    JsonBson::JsonBson(const Json::Value &val) {
        Json::FastWriter writer;
        m_data = writer.write(val);
    }


    JsonBson::JsonBson(const mongo::BSONObj &obj) {
        m_data = mongo::tojson(obj);
    }


    JsonBson::JsonBson(const std::string &str) {
        m_data = str;
    }

    JsonBson::~JsonBson() {

    }

    JsonBson::operator Json::Value() const {
        Json::Value root;
        Json::Reader parser;
        parser.parse(m_data, root);
        return root;
    }

    JsonBson::operator mongo::BSONObj() const {
        return mongo::fromjson(m_data);
    }



    JsonBson &JsonBson::operator=(const Json::Value &val) {
        Json::FastWriter writer;
        m_data = writer.write(val);
    }

    JsonBson &JsonBson::operator=(const mongo::BSONObj &obj) {
        m_data = mongo::tojson(obj);
    }

    JsonBson &JsonBson::operator=(const std::string &str) {
        m_data = str;
        return *this;
    }

    const std::string &JsonBson::str() const {
        return m_data;
    }
}