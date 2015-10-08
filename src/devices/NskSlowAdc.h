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
		virtual bool handleRequest (Request & req, Response & resp);

	protected:

		Socket m_controlSock;
		Socket m_dataSock;
	private:
	};
}

#endif //PROJECT_NSKSLOWADC_H
