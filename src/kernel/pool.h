#pragma once

#include "../helpers/socket.h"
#include "PollingBuffer.h"

namespace Flug {

	static const uint32_t DefaultEvMask = (EPOLLIN | EPOLLOUT);

	/** ConnectionPool class is a singleton */
	class ConnectionPool {
		public:

			ConnectionPool();
			virtual ~ConnectionPool();

			void addSocket (const Socket & sink, uint32_t evmask = DefaultEvMask);
			void updateSocket(const Socket &sink, uint32_t evmask = DefaultEvMask);
			void removeSocket (const Socket &sink);
			void registerCallback();

			size_t wait (epoll_event * evs, size_t maxEvs);
		protected:
		private:
			int m_poll;
			std::list<PollingBuffer*> m_buffers;
	};





}
