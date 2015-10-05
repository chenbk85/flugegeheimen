//
// Created by user on 9/29/15.
//

#include "../stdafx.h"
#include "Module.h"

#define MAX_MODULE_QUEUE_DEPTH 100
#define MODULE_REQUEST_THROTTLING 10

namespace Flug {
	Module::Module() :
	m_quitState(false){
		m_thread = new std::thread(&Module::moduleProc, this);
	}

	Module::~Module() { }

	void Module::moduleProc() {
		if (!this->initModule()) {
			//write to log
		}
		while (!this->m_quitState) {
			this->mainLoop();
		}
		if (!this->destroyModule()) {
			//write to log
		}
	}

	void Module::mainLoop() {
		handleIncomingRequests();
		std::this_thread::sleep_for(
				std::chrono::milliseconds(MODULE_REQUEST_THROTTLING));
	}


	void Module::handleIncomingRequests() {
		Request request;
		Response response;
		while (m_inQueue.pop(request)) {
			if (!handleRequestWraper(request, response)) {
				//handle error? yep. write to log.
			}
			m_outQueue.push(response);
		}
	}

	bool Module::pushRequest(Request & req) {
		std::cout <<
			"\n[" << req.m_string << "]" << "(" << req.m_id << ")\n" << std::endl;
		return m_inQueue.push(req);
	}

	bool Module::popResponse(Response & resp) {
		return m_outQueue.pop(resp);
	}

	bool Module::handleRequestWraper(Request &req, Response &resp) {
		if (!req.m_parsed) {
			throw std::runtime_error("Non-parsed request sent to module handling proc");
		}

		resp.m_id = req.m_id;
		resp.m_pbuf = req.m_pbuf;

		return handleRequest(req, resp);
	}

}