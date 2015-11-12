//
// Created by user on 30.10.15.
//

#ifndef PROJECT_DEVICEPROXY_H
#define PROJECT_DEVICEPROXY_H

#include "Request.h"
#include "Response.h"

namespace Flug {
    class DeviceProxy {
    public:
        DeviceProxy ();
        virtual ~DeviceProxy ();

        bool proxyRequest (Request & req, Response & resp);
        void proxyResponse (Request & req, Response & resp);


    protected:
    };
}


#endif //PROJECT_DEVICEPROXY_H
