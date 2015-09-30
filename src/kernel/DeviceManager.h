//
// Created by user on 9/29/15.
//

#pragma once

#include "DeviceInterface.h"
#include "Module.h"

namespace Flug {
	class DeviceManager : public Module {
	public:

		DeviceManager();

		virtual ~DeviceManager();

		void registerDevice(const std::string &deviceName, DeviceInterface *device);

		virtual bool initModule ();
		virtual bool destroyModule ();
		virtual bool handleRequest (const std::string & request, std::string & response);

	protected:
	private:
	};

}

