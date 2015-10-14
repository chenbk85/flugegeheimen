//
// Created by user on 13.10.15.
//

#ifndef PROJECT_DEVICEBUILDER_H
#define PROJECT_DEVICEBUILDER_H

#include "../stdafx.h"
#include "DeviceDriver.h"

namespace Flug {

    class DeviceConstructor {
    public:
        virtual DeviceDriver *createInstance(const std::string &instanceName, const std::string & devType) = 0;
    protected:
    private:
    };

    template<class DevDrv>
    class DeviceConstructorInstance : public DeviceConstructor {
    public:
        DeviceDriver *createInstance(const std::string &instanceName, const std::string & devType) {
            return static_cast<DeviceDriver *> (new DevDrv(instanceName, devType));
        };
    };

    class DeviceBuilder {
    public:
        DeviceDriver * createDeviceInstance (const std::string & driverName, const std::string & instanceName) {
            return m_drivers[driverName]->createInstance(instanceName, driverName);
        };

        template <class DevDrv> void registerDeviceDriver (const std::string & driverName) {
            m_drivers[driverName] = static_cast<DeviceConstructor*>(new DeviceConstructorInstance<DevDrv>());
        };
    protected:
        std::map<std::string, DeviceConstructor *> m_drivers;
    };

}


#endif //PROJECT_DEVICEBUILDER_H
