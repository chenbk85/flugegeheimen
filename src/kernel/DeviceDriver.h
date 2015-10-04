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

		virtual bool isOnline () = 0;

	protected:
	private:
	};

}
