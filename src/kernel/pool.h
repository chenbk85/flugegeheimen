#pragma once

#include "../helpers/socket.h"

namespace Flug {

	/** Pool class is a singleton */
	class Pool {
		public:
			static Pool & getInstance ();
			virtual ~Pool ();

			void addSinkSocket (const Socket & sink);
			void updateSinkSocket (const Socket & sink);
			void removeSinkSocket ();
			void registerSinkCallback ();

			void addDeviceDescriptor (int descr, int ops);
			void removeDeviceDescriptor (int descr);
			void registerDeviceCallback ();

			void wait ();
		protected:
		private:
			int m_poll;

			int m_sinkSock;

			epoll_event m_sinkEventMask;

			Pool ();
	};





}
