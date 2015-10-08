//
// Created by user on 9/30/15.
//

#pragma once

#include "Module.h"

namespace Flug {

	class DeviceDriver : public Module {
	public:
		DeviceDriver ();
		virtual ~DeviceDriver();


		virtual bool initModule ();
		virtual bool destroyModule ();
		virtual bool handleRequest (Request & req, Response & resp);

	protected:
	private:
	};

}
