//
// Created by user on 9/29/15.
//

#pragma once

#include "DeviceDriver.h"
#include "Module.h"
#include "RemoteDispatcher.h"

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
        virtual bool rebootModule ();


		bool initDevices (RemoteDispatcher * dispatcher, Json::Value & devList);

        void getDevicesList (std::list<std::pair<std::string, std::string> > & devices);

	protected:

		bool handleGetDeviceTypesList (Request & req, Response & resp);
		bool handleGetDevicesOfType (Request & req, Response & resp);
		bool handleGetDevicesList (Request & req, Response & resp);


		std::map<std::string, DeviceDriver * > m_devices;

	private:
	};

}

