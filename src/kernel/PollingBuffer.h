//
// Created by user on 8/11/15.
//

#pragma once

#include "../stdafx.h"
#include "../helpers/socket.h"

namespace Flug {

	class PollingBuffer {
	public:
		PollingBuffer();

		/*Do not implement*/

		PollingBuffer(int sock);

		virtual ~PollingBuffer();

		void sendMessage(const std::string &msg);

		bool recvMessage(std::string &str);

		void performTransimission(uint32_t event);

	protected:
		void pushMessages();

		void popMessages();

		void performRead();

		void performWrite();

		bool checkForMessages();

		int m_socket;
		std::vector<char> m_inputData;
		std::vector<char> m_outputData;
		std::list<std::string> m_inbox;
		std::list<std::string> m_outbox;
		size_t m_outputPtr;
        size_t m_ballance;

	private:
	};


}