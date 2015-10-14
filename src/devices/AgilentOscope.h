//
// Created by user on 10/4/15.
//

#pragma once

#include "../kernel/DeviceDriver.h"


namespace Flug {
	class AgilentOscope : public DeviceDriver {
	public:
		AgilentOscope () = delete;
		AgilentOscope (const std::string & deviceInstanceName, const std::string & devType);
		virtual ~AgilentOscope();


		virtual bool initModule ();
		virtual bool destroyModule ();
		virtual bool handleRequest (Request & req, Response & resp);

	protected:
		Socket m_sock;
		bool m_connected;

		bool command (const std::string & str);
		bool commandUnsafe (const std::string & str);
		bool request (const std::string & req, std::string & resp);
		void getWordData (std::vector<int16_t> & data);
		double getFloating (const std::string & num);
		void tryUpdateVariable (const std::string & cmd, const std::string & req,
								const std::string & val, std::string & newVal);


		//request handling here
		void handleGetDataRequest (Request & req, Json::Value & root);
		void handleCommandRequest (Request & req, Json::Value & root);
		void handleRequestRequest (Request & req, Json::Value & root);
		void handleIsOnlineRequest (Request & req, Json::Value & root);


	private:
		std::vector<char> m_accumulator;

		size_t parseWordRespHeader (const char * buf,
									size_t & dataStart);
		void parseWordsFromBuf (const char * buf, size_t size,
						   std::vector<int16_t> & data);
	};
}

