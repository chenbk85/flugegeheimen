//
// Created by user on 8/14/15.
//

#pragma once

#include "../kernel/DeviceDriver.h"

namespace Flug {
	class DummyDevice : public DeviceDriver {
	public:
		DummyDevice() = delete;
		DummyDevice(const std::string & deviceInstanceName, const std::string & devType);

		virtual ~DummyDevice();

		void getData (char * buf, size_t size);

		virtual bool initModule ();
		virtual bool destroyModule ();
		virtual bool handleRequest (Request & req, Response & resp);
		virtual bool isOnline ();

		void dataToJsonArray(const char *data, size_t size, std::string &jsonArray);
	};

}

