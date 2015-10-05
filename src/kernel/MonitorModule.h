//
// Created by user on 10/5/15.
//

#ifndef PROJECT_MONITORMODULE_H
#define PROJECT_MONITORMODULE_H

#include "Module.h"

namespace Flug {
	class MonitorModule : public Module {
	public:
		virtual bool initModule ();
		virtual bool destroyModule ();
		virtual bool handleRequest (Request & req, Response & resp);

	protected:
	private:
	};
}


#endif //PROJECT_MONITORMODULE_H
