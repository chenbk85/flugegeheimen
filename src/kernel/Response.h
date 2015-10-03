//
// Created by user on 10/3/15.
//

#ifndef PROJECT_RESPONSE_H
#define PROJECT_RESPONSE_H

#include "PollingBuffer.h"

namespace Flug {
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
		}

		Response (const Response & oth) :
				m_id(oth.m_id), m_json(oth.m_json), m_string(oth.m_string),
				m_pbuf(oth.m_pbuf) {
		}



		std::string m_string;
		Json::Value m_json;
		uint64_t m_id;
		PollingBuffer * m_pbuf;
	protected:
	private:
	};
}

#endif //PROJECT_RESPONSE_H
