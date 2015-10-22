#include "../stdafx.h"
#include "response_parser.h"

namespace Flug {
	ResponseParser::ResponseParser(const uint8_t*data, size_t size) :
			m_data(data), m_size(size), m_offset(0) {

	};

	ResponseParser::ResponseParser() {
	}

	ResponseParser::~ResponseParser() {

	};



	void ResponseParser::flush() {
		m_offset = 0;
	};

	void ResponseParser::setData(const uint8_t*data, size_t size) {
		m_data = data;
		m_size = size;
		m_offset = 0;
	};

}