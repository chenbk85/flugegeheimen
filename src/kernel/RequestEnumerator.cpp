//
// Created by user on 10/3/15.
//

#include "RequestEnumerator.h"

namespace Flug {
	RequestEnumerator::RequestEnumerator() {
		m_counter.store(0, std::memory_order_relaxed);
	}
	RequestEnumerator::~RequestEnumerator() { }
	RequestEnumerator& RequestEnumerator::getInstance() {
		static RequestEnumerator _inst;
		return _inst;
	}
	uint64_t RequestEnumerator::getRequestId() {
		return m_counter.fetch_add((uint64_t)1, std::memory_order_relaxed);
	}
}