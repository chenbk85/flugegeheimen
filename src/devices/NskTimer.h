//
// Created by user on 14.10.15.
//

#ifndef PROJECT_NSKTIMER_H
#define PROJECT_NSKTIMER_H

#include "../kernel/DeviceDriver.h"

namespace Flug {
    class NskTimer : public DeviceDriver {
    public:
        NskTimer() = delete;
        NskTimer (const std::string & deviceInstanceName, const std::string & devType);
        virtual ~NskTimer();

        bool initModule ();
        bool destroyModule ();
        bool handleRequest (Flug::Request & req, Flug::Response & resp);
        bool loadConfig(Json::Value &config);

    protected:
        int m_channelsNumber;
        std::vector<uint16_t> m_delays;
        std::string m_addr;
        std::string m_port;
        Socket m_sock;
        bool m_connected;

        bool handleIsOnline (Flug::Request & req, Flug::Response & resp);
        bool handleSetChannelDelay (Flug::Request & req, Flug::Response & resp);
        bool handleGetChannelsDelays (Flug::Request & req, Flug::Response & resp);
    };
}


#endif //PROJECT_NSKTIMER_H
