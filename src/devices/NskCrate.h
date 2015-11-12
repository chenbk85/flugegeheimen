//
// Created by user on 09.11.15.
//

#ifndef PROJECT_NSKCRATE_H
#define PROJECT_NSKCRATE_H

#include "../kernel/DeviceDriver.h"

namespace Flug {
    class NskCrate : public DeviceDriver {
    public:
        NskCrate() = delete;

        NskCrate(const std::string &devName, const std::string &devType);

        virtual ~NskCrate();

        virtual bool initModule();

        virtual bool destroyModule();

        virtual bool handleRequest(Request &req, Response &resp);

        virtual bool loadConfig(Json::Value &config);

    protected:
        bool handleGetData (Request & req, Response & resp);

        std::vector<std::string> m_adcs;
    };
}


#endif //PROJECT_NSKCRATE_H
