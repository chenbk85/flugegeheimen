//
// Created by user on 9/30/15.
//

#pragma once

#include "Module.h"

namespace Flug {

	class DeviceDriver : public Module {
	public:
		DeviceDriver () = delete;
		DeviceDriver (const std::string & deviceInstance, const std::string & deviceType);
		virtual ~DeviceDriver();


		virtual bool loadConfig (Json::Value & config);
		virtual bool initModule ();
		virtual bool destroyModule ();
		virtual bool handleRequest (Request & req, Response & resp);
        virtual bool rebootModule ();

		const std::string & getDeviceName ();
		const std::string & getDeviceType ();

	protected:
		std::string m_deviceName;
		std::string m_deviceType;

	private:
	};

}
