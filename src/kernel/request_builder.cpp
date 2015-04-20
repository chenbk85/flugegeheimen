#include "../stdafx.h"
#include "request_builder.h"



namespace Flug {

	RequestBuilder::RequestBuilder() {

	};

	RequestBuilder::~RequestBuilder() {

	};

	const char * RequestBuilder::data() const {
		return reinterpret_cast<const char *>(m_data.data());
	};

	size_t RequestBuilder::size() const {
		return m_data.size();
	};

	void RequestBuilder::flush() {
		m_data.clear();
	};

	RequestBuilder & RequestBuilder::push (const char * data, size_t size) {
		m_data.insert(m_data.end(), data, data+size);
		return *this;
	}

}