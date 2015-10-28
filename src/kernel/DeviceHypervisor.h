//
// Created by user on 28.10.15.
//

#ifndef PROJECT_DEVICEHYPERVISOR_H
#define PROJECT_DEVICEHYPERVISOR_H

#include "DeviceDriver.h"

namespace Flug {
    class DeviceHypervisor {
    public:
        DeviceHypervisor (DeviceDriver * device);
        virtual ~DeviceHypervisor();
    protected:

        DeviceDriver * m_device;
    private:
    };
}


#endif //PROJECT_DEVICEHYPERVISOR_H
