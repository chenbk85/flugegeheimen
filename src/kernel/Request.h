//
// Created by user on 10/3/15.
//

#ifndef PROJECT_REQUEST_H
#define PROJECT_REQUEST_H

#include "RequestEnumerator.h"
#include "PollingBuffer.h"

namespace Flug {

    /**
     * Request structure contains the requests to module passed by other
     * modules and/or user interfaces
     */
	struct Request {
		Request() {};

		virtual ~Request() {};

		Request (const std::string & request, PollingBuffer * pbuf) {
			Json::Reader parser;
			m_string = request;
			if (parser.parse(request, m_json)) {
				m_parsed = true;
			} else {
				m_parsed = false;
			}
			RequestEnumerator & enumer = RequestEnumerator::getInstance();
			m_id = enumer.getRequestId();
			m_pbuf = pbuf;
            m_specificSender = false;
		}

        Request (const Request & oth) :
				m_id(oth.m_id), m_json(oth.m_json), m_string(oth.m_string),
				m_parsed(oth.m_parsed), m_pbuf(oth.m_pbuf),
                m_specificSender(oth.m_specificSender), m_sender(oth.m_sender) {
		}

		Request &operator= (const Request & oth) {
			m_id = oth.m_id;
			m_parsed = oth.m_parsed;
			m_string = oth.m_string;
			m_json = oth.m_json;
			m_pbuf = oth.m_pbuf;
		}

		std::string m_string;
		Json::Value m_json;
		bool m_parsed;
		uint64_t m_id;
        bool m_specificSender;
        std::string m_sender;
		PollingBuffer * m_pbuf;
	};

}

#endif //PROJECT_REQUEST_H
