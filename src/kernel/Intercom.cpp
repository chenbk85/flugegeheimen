//
// Created by user on 09.11.15.
//

#include "../stdafx.h"
#include "Intercom.h"

#define INTERCOM_QUEUE_DEPTH 0x800

namespace Flug {

    Intercom& Intercom::getInstance() {
        static Intercom _inst;
        return _inst;
    }

    Intercom::Intercom() :
    m_queue(INTERCOM_QUEUE_DEPTH) {

    }

    Intercom::~Intercom() {

    }

    void Intercom::sendRequest(const std::string &recipient, const std::string &caller, Request &req) {
        IntercomMsg * msg = new IntercomMsg();
        msg->m_recipient = recipient;
        msg->m_sender = caller;
        msg->m_req = req;

        m_queue.push(msg);
    }

    Intercom::IntercomMsg* Intercom::popMessage() {
        IntercomMsg * msg;
        m_queue.pop(msg);
        return msg;
    }

    bool Intercom::hasMessages() {
        return !m_queue.empty();
    }
}

