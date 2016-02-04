//
// Created by user on 03.02.16.
//

#pragma once
#include "../stdafx.h"
#include "../kernel/DeviceDriver.h"

namespace Flug {
    class DummyCrate : public DeviceDriver {
    public:
        DummyCrate() = delete;
        DummyCrate(const std::string & devName, const std::string & devType);
        virtual ~DummyCrate();

        virtual bool initModule ();
        virtual bool destroyModule ();
        virtual bool handleRequest (Request & req, Response & resp);
        virtual bool isOnline ();
        virtual bool loadConfig(Json::Value &config);

    protected:
        std::vector<std::string> m_devs;
    private:
    };
}
