//
// Created by user on 09.11.15.
//

#ifndef PROJECT_INTERCOM_H
#define PROJECT_INTERCOM_H

#include "Request.h"
#include "Response.h"
#include "Module.h"

namespace Flug {
    class Intercom {
    public:

        struct IntercomMsg {
        public:
            std::string m_sender;
            std::string m_recipient;
            Request m_req;
            Response m_resp;
        };

        static Intercom & getInstance ();

        virtual ~Intercom();

        void sendRequest (const std::string & recipient, const std::string & caller, Request & req);

        IntercomMsg * popMessage ();

        bool hasMessages ();

    protected:
        boost::lockfree::queue<IntercomMsg*> m_queue;
    private:
        Intercom ();
    };
}


#endif //PROJECT_INTERCOM_H
