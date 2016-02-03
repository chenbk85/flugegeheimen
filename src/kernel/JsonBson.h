//
// Created by user on 29.10.15.
//

#ifndef PROJECT_JSONBSON_H
#define PROJECT_JSONBSON_H

#include "Response.h"

namespace Flug {

    class JsonBson {
    public:
        JsonBson();
        JsonBson (const mongo::BSONObj & obj);
        JsonBson (const Json::Value & val);
        JsonBson (const std::string & str);
        virtual ~JsonBson();

        JsonBson &operator=(const Json::Value & val);
        JsonBson &operator=(const mongo::BSONObj & obj);
        JsonBson &operator=(const std::string & str);

        operator Json::Value() const;
        operator mongo::BSONObj() const;
        const std::string & str() const;

    protected:
        std::string m_data;
    };

}


#endif //PROJECT_JSONBSON_H
