//
// Created by user on 10/4/15.
//

#pragma once

#include "../kernel/DeviceDriver.h"


namespace Flug {
	class AgilentOscope : public DeviceDriver {
	public:
		AgilentOscope ();
		virtual ~AgilentOscope();


		virtual bool initModule ();
		virtual bool destroyModule ();
		virtual bool handleRequest (Request & req, Response & resp);
		virtual bool isOnline ();

	protected:
		Socket m_sock;
		bool m_connected;
	private:
	};
}

