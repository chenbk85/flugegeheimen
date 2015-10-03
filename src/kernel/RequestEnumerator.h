//
// Created by user on 10/3/15.
//

#ifndef PROJECT_REQESTENUMERATOR_H
#define PROJECT_REQESTENUMERATOR_H

#include <stdint-gcc.h>
#include <atomic>

namespace Flug {
	class RequestEnumerator {
	public:
		static RequestEnumerator &getInstance();
		uint64_t getRequestId ();
		virtual ~RequestEnumerator();

	protected:
	private:
		RequestEnumerator();
		std::atomic<uint64_t> m_counter;
	};
}


#endif //PROJECT_REQESTENUMERATOR_H
