#include "../stdafx.h"
#include "timer.h"
#include "../helpers/xml.h"
#include "../kernel/request_builder.h"
#include "../kernel/response_parser.h"

namespace Flug {

	namespace Nsk {
		Timer::Timer () {

		}

		Timer::~Timer() {

		}

		void Timer::loadConfig(const std::string & configXml) {
			Flug::Xml xml;
			xml.loadData(configXml);
			xml.get("/flugegeheimen/timer@addr", m_addr);
			xml.get("/flugegeheimen/timer@port", m_port);
			xml.get("/flugegeheimen/timer@channelsNumber", m_channelsNumber);

			for (size_t i = 0; i < m_channelsNumber; i++) {
				ChannelPrefences prefs;
				prefs.m_chan = i;
				std::string chanNo;
				std::stringstream ss;
				ss << i;
				ss >> chanNo;
				xml.get("/flugegeheimen/timer/channel_" + chanNo + "@delay", prefs.m_delay);
				m_prefs.push_back(prefs);
			}
		}

		void Timer::connect() {
			m_sock.connect(m_addr, m_port);
		}

		void Timer::disconnect() {
			m_sock.disconnect();
		}

		void Timer::setChannelDelay(uint8_t chanNo, uint16_t delay) {
			Flug::RequestBuilder req;
			Flug::ResponseParser res;

			req << ((uint8_t) 0x80) << ((uint8_t) chanNo) << ((uint8_t) 0x2) << ((uint16_t) delay);
			m_sock.send(req.data(), req.size());

			char buf[4];
			uint8_t numReg, cmd;
			uint16_t val;

			m_sock.recv(buf, 4);
			res.setData(buf, 4);
			res >> numReg >> cmd >> val;
		}

		void Timer::setDefaultDelays() {
			for (auto iter: m_prefs) {
				setChannelDelay(iter.m_chan, iter.m_delay);
			}
		}
	}

}