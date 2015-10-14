//
// Created by user on 10/5/15.
//

#include "../stdafx.h"
#include "MonitorModule.h"

namespace Flug {

	MonitorModule::MonitorModule() :
	Module("MonitorModule") { }

	MonitorModule::~MonitorModule() { }

	bool MonitorModule::initModule() {
		return true;
	}

	bool MonitorModule::destroyModule() {
		return true;
	}

	bool MonitorModule::handleRequest(Request &req, Response &resp) {
		Json::Value root;
		root["status"] = "success";
		resp = root;
		return true;
	}
}