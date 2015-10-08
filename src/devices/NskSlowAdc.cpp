//
// Created by user on 10/8/15.
//

#include "../stdafx.h"
#include "NskSlowAdc.h"


namespace Flug {
	NskSlowAdc::NskSlowAdc() {
	}


	NskSlowAdc::~NskSlowAdc() { }

	bool NskSlowAdc::initModule() {
		return true;
	}

	bool NskSlowAdc::handleRequest(Request &req, Response &resp) {
		return true;
	}

	bool NskSlowAdc::destroyModule() {
		return true;
	}

}