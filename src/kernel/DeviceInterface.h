//
// Created by user on 9/29/15.
//

#pragma once

namespace Flug {
	class DeviceInterface {
	public:
		virtual bool initDevice () = 0;
		virtual std::string handleRequest (const std::string & req) = 0;
	};
}