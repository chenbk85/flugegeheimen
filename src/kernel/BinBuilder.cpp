//
// Created by user on 10/12/15.
//

#include "../stdafx.h"
#include "BinBuilder.h"

namespace Flug {
	BinBuilder::BinBuilder() {
	}

	BinBuilder::~BinBuilder() {
	}

	void BinBuilder::flush() {
		m_data.erase(m_data.begin(), m_data.end());
	}

	void BinBuilder::getData(std::vector<uint8_t> &data) {
		data = m_data;
	}

	void BinBuilder::setData(const uint8_t *ptr, size_t size) {
		m_data.assign(ptr, ptr + size);
	}
}