//
// Created by user on 10/8/15.
//

#ifndef PROJECT_NSKSLOWADC_H
#define PROJECT_NSKSLOWADC_H

#include "../kernel/DeviceDriver.h"

namespace Flug {
	class NskSlowAdc : public DeviceDriver {
	public:
		NskSlowAdc ();
		virtual ~NskSlowAdc ();

		virtual bool initModule ();
		virtual bool destroyModule ();
		virtual bool handleRequest (Flug::Request & req, Flug::Response & resp);

	protected:

		struct Command {
			uint8_t m_register;
			uint16_t m_value;
		};
		struct Response {
			uint8_t m_register;
			uint16_t m_value;
		};


		bool connect ();
		void disconnect ();


		//control port (2001)
		bool performCommand (const Command & cmd, Response & resp);

		void loadMatrix (const std::string & path);
		bool setHighVoltage ();
		bool setPagesCount ();
		void dropAddrCounter ();
		void allowRead ();
		void allowStart ();

		//info port (2000)
		void readDataPages();

		//adc port (2002)
		void readAcpData ();



		Socket m_controlSock;
		Socket m_infoSock;
		Socket m_adcSock;
	private:
	};
}

#endif //PROJECT_NSKSLOWADC_H
