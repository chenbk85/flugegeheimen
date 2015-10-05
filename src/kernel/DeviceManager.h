//
// Created by user on 9/29/15.
//

#pragma once

#include "DeviceDriver.h"
#include "Module.h"

namespace Flug {
	class DeviceManager : public Module {
	public:

		DeviceManager();

		virtual ~DeviceManager();

		void registerDevice(const std::string &deviceName, DeviceDriver *device);
		DeviceDriver *operator[](const std::string &deviceName);

		virtual bool initModule ();
		virtual bool destroyModule ();
		virtual bool handleRequest (Request & req, Response & resp);

	protected:
		std::map<std::string, DeviceDriver * > m_devices;


	private:
	};

}

