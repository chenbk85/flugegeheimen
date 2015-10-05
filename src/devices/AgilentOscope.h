//
// Created by user on 10/4/15.
//

#pragma once

#include "../kernel/DeviceDriver.h"


namespace Flug {
	class AgilentOscope : public DeviceDriver {
	public:
		AgilentOscope ();
		virtual ~AgilentOscope();


		virtual bool initModule ();
		virtual bool destroyModule ();
		virtual bool handleRequest (Request & req, Response & resp);
		virtual bool isOnline ();

	protected:
		Socket m_sock;
		bool m_connected;

		bool command (const std::string & str);
		bool request (const std::string & req, std::string & resp);
		void getWordData (std::vector<int16_t> & data);

	private:
		std::vector<int16_t> m_lastData;


		size_t parseWordRespHeader (const char * buf,
									size_t & dataStart);
		void parseWordsFromBuf (const char * buf, size_t size,
						   std::vector<int16_t> & data);
	};
}

