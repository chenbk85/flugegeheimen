//
// Created by user on 10/8/15.
//

#ifndef PROJECT_NSKSLOWADC_H
#define PROJECT_NSKSLOWADC_H

#include "../kernel/DeviceDriver.h"
#include "../kernel/request_builder.h"

namespace Flug {
	class NskSlowAdc : public DeviceDriver {
	public:
		NskSlowAdc () = delete;
		NskSlowAdc (const std::string & deviceInstanceName, const std::string & devType);
		virtual ~NskSlowAdc ();

		virtual bool initModule ();
		virtual bool destroyModule ();
		virtual bool handleRequest (Request & req, Response & resp);
		virtual bool loadConfig (Json::Value & config);

	protected:

		bool handleAllowStart (Request & req, Response & resp);
		bool handleAllowRead (Request & req, Response & resp);
		bool handleGetData (Request & req, Response & resp);

	private:
		struct Cmd {
			Cmd (uint8_t reg, uint16_t val) : m_register(reg), m_value(val) {};
			uint8_t m_register;
			uint16_t m_value;
		};

		struct Resp {
			uint8_t m_register;
			uint16_t m_value;
		};


		bool connect ();
		void disconnect ();


		//control port (2001)
		bool performCommand(Socket &sock, const Cmd &cmd, Resp &resp, uint8_t chanNo = 0x2);

		void loadFirmware ();
		void setPagesCount (uint16_t pages);  //max 64 pages
		void setHighVoltage (uint8_t chan, bool enable);
		void dropAddrCounter ();
		void allowRead ();
		void allowStart ();

		//info port (2000)
		void readDataPages();
		void readDataPage(uint16_t * data);

		//adc port (2002)
		void readDacData ();


		void DO_THE_MAGIC_BITCH ();


		Socket m_controlSock;
		Socket m_infoSock;
		Socket m_dacSock;
		uint16_t m_data[0x8][0x400];
		size_t m_pagesCount;

		//configuration

		std::string m_addr;
		std::string m_infoPort;
		std::string m_controlPort;
		std::string m_dacPort;
		std::string m_firmwareLocation;
		std::vector<uint8_t> m_firmware;
		bool m_highVoltageEnabled;

		bool handleGetAllData(Request &req, Response &resp);

		bool handleGetAmpVoltages(Request &req, Response &resp);
	};
}

#endif //PROJECT_NSKSLOWADC_H
