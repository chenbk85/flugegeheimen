//
// Created by user on 9/29/15.
//

#pragma once

#include "Module.h"

namespace Flug {

	class Dispatcher {
		struct HandlerRecord {
			HandlerRecord (const std::string & name, Module * module) :
					m_name(name), m_module(module) {};

			HandlerRecord () :
					m_name("NULLMODULE"), m_module(nullptr){
			}

			HandlerRecord &operator=(const HandlerRecord & oth) {
				m_name = oth.m_name;
				m_module = oth.m_module;
				return *this;
			}

			std::string m_name;
			Module * m_module;
		};
	public:
		Dispatcher();

		virtual ~Dispatcher();

		void registerModule(const std::string &moduleName, Module *module);
		bool dispatchRequest(const std::string & request);
		bool checkForResponses (std::string & response);


	protected:
	private:
		std::map<std::string, HandlerRecord> m_modules;
	};

}
