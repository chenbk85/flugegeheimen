#include "../stdafx.h"
#include "response_parser.h"

namespace Flug {
	ResponseParser::ResponseParser(const char *data, size_t size) :
			m_data(reinterpret_cast<const uint8_t *>(data)), m_size(size), m_offset(0) {

	};

	ResponseParser::ResponseParser() {
	}

	ResponseParser::~ResponseParser() {

	};



	void ResponseParser::flush() {
		m_offset = 0;
	};

	void ResponseParser::setData(const char *data, size_t size) {
		m_data = reinterpret_cast<const uint8_t *>(data);
		m_size = size;
		m_offset = 0;
	};

}