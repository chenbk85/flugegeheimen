//
// Created by user on 8/14/15.
//

#pragma once

namespace Flug {
	class DummyDevice {
	public:
		DummyDevice();

		virtual ~DummyDevice();

		void getData (char * buf, size_t size);

	};

}

