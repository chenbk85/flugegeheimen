//
// Created by user on 10/3/15.
//

#ifndef PROJECT_RESPONSE_H
#define PROJECT_RESPONSE_H

#include "PollingBuffer.h"


namespace Flug {
    class Module;

	struct Response {
	public:
		Response() {};

		virtual ~Response() {};

		Response(Json::Value root, uint64_t id, PollingBuffer * pbuf) {
			m_json = root;
			Json::FastWriter writer;
			std::stringstream ss;
			ss << writer.write(m_json);
			m_string = ss.str();
			m_id = id;
			m_pbuf = pbuf;
            m_local = false;
            m_module = nullptr;
		}

        Response(Json::Value root, uint64_t id, Module * module) {
            m_json = root;
            Json::FastWriter writer;
            std::stringstream ss;
            ss << writer.write(m_json);
            m_string = ss.str();
            m_id = id;
            m_pbuf = nullptr;
            m_local = true;
            m_module = module;
        }

		Response (const Response & oth) :
				m_id(oth.m_id), m_json(oth.m_json), m_string(oth.m_string),
				m_pbuf(oth.m_pbuf), m_local(oth.m_local), m_module(oth.m_module) {
		}

        Response &operator=(const Response & oth) {
            m_id = oth.m_id;
            m_json = oth.m_json;
            m_string = oth.m_string;
            m_module = oth.m_module;
            m_pbuf = oth.m_pbuf;
            m_local = oth.m_local;
        }

		Response &operator=(Json::Value & val) {
			Json::FastWriter writer;
			m_json = val;
			m_string = writer.write(val);
		}


        bool m_local; //Is it a local request
		std::string m_string;
		Json::Value m_json;
		uint64_t m_id;
		PollingBuffer * m_pbuf; //In case of remote request
        Module * m_module; //In case of local request

	protected:
	private:
	};
}

#endif //PROJECT_RESPONSE_H
