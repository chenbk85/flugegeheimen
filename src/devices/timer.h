#pragma once

#include <stdint.h>
#include "../helpers/socket.h"

namespace Flug {
	namespace Nsk {
		class Timer {
		public:

			struct ChannelPrefences {
				uint8_t m_chan;
				uint16_t m_delay;
			};
			Timer ();
			virtual ~Timer ();

			void loadConfig (const std::string & configXml);

			void connect ();
			void disconnect ();

			void setDefaultDelays ();
			void setChannelDelay (uint8_t chanNo, uint16_t delay);


		protected:
			std::string m_addr;
			std::string m_port;
			Flug::Socket m_sock;
			size_t m_channelsNumber;
			std::list<Flug::Nsk::Timer::ChannelPrefences> m_prefs;
		private:
		};
	}

}