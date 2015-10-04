//
// Created by user on 10/4/15.
//

#include "../stdafx.h"
#include "AgilentOscope.h"

namespace Flug {
	AgilentOscope::AgilentOscope() {
		m_connected = false;
	}

	AgilentOscope::~AgilentOscope() {
	}

	bool AgilentOscope::initModule() {
		try {
			m_sock.connect("172.16.13.238", "5025");
			m_connected = true;
			return true;
		} catch (std::runtime_error & err) {
			std::cout << err.what() << std::endl;
			m_connected = false;
			return false;
		}
	}

	bool AgilentOscope::destroyModule() {
		return true;
	}

	bool AgilentOscope::handleRequest(Request &req, Response &resp) {
		return true;
	}

	bool AgilentOscope::isOnline() {
		return m_connected;
	}
}