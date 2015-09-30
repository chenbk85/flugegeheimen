//
// Created by user on 9/29/15.
//

#ifndef PROJECT_REQUESTHANDLERINTERFACE_H
#define PROJECT_REQUESTHANDLERINTERFACE_H

namespace Flug {
	class RequestHandlerInterface {
	public:
		virtual std::string handle (const std::string & request) = 0;
	};
}

#endif //PROJECT_REQUESTHANDLERINTERFACE_H
