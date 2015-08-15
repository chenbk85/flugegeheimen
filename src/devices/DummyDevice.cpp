//
// Created by user on 8/14/15.
//

#include "../stdafx.h"
#include "DummyDevice.h"


namespace Flug {
	DummyDevice::DummyDevice() {
		srand(time(NULL));
	}

	DummyDevice::~DummyDevice() {
	}

	void DummyDevice::getData(char *buf, size_t size) {
		for (size_t i = 0; i < size; i++) {
			buf[i] = (char) (rand() % 0xFF);
		}
	}
}