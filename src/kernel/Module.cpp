//
// Created by user on 9/29/15.
//

#include "../stdafx.h"
#include "Module.h"

#define MAX_MODULE_QUEUE_DEPTH 100
#define MODULE_REQUEST_THROTTLING 10

namespace Flug {
	Module::Module() :
	m_quitState(false), std::thread(&Module::moduleProc, this),
	m_inQueue(MAX_MODULE_QUEUE_DEPTH),
	m_outQueue(MAX_MODULE_QUEUE_DEPTH) {

	}

	Module::~Module() { }

	void Module::moduleProc() {
		if (!initModule()) {
			//write to log
		}
		while (!m_quitState) {
			mainLoop();
		}
		if (!destroyModule()) {
			//write to log
		}
	}

	void Module::mainLoop() {
		handleIncomingRequests();
		std::this_thread::sleep_for(
				std::chrono::milliseconds(MODULE_REQUEST_THROTTLING));
	}


	void Module::handleIncomingRequests() {
		std::string request, response;
		while (m_inQueue.pop(request)) {
			if (!handleRequest(request, response)) {
				//handle error? yep. write to log.
			}
			m_outQueue.push(response);
		}
	}

	bool Module::pushRequest(const std::string & req) {
		return m_inQueue.push(req);
	}

	bool Module::popResponse(std::string & resp) {
		return m_outQueue.pop(resp);
	}

}