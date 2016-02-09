//
// Created by user on 8/11/15.
//

#include "../stdafx.h"
#include "PollingBuffer.h"

static const size_t BufSize = 1024 * 10;

namespace Flug {

	PollingBuffer::PollingBuffer(int sock)
			: m_socket(sock), m_ballance(0) {
	}

	PollingBuffer::~PollingBuffer() {
	}

	void PollingBuffer::performTransimission(uint32_t event) {
        if (event & EPOLLOUT && m_outputData.size() != 0) {
            std::cout << "#Writing data" << std::endl;
            performWrite();
        }

        pushMessages();
        popMessages();

        if (m_ballance == 0 && (event & EPOLLIN)) {
			std::cout << "#Reading data" << std::endl;
			performRead();
		}

	}

	void PollingBuffer::performRead() {
		if (m_socket == -1) {
			throw std::runtime_error("Invalid socket");
		}

		char buf[BufSize];
		size_t recvd;
		Socket sock(m_socket);
		bool ret = true;
		while (ret) {
			ret = sock.recvNonblock(reinterpret_cast<uint8_t*>(buf), BufSize, recvd);
			if (ret) {
				if (!recvd) {
					std::cout << "#Disconnected (read)" << std::endl;
					sock.disconnect();
					break;
					//throw std::runtime_error("Disconnected!");
				}
				m_inputData.insert(m_inputData.end(), buf, buf + recvd);
			}
		}
	}

	void PollingBuffer::performWrite() {
		if (m_socket == -1) {
			throw std::runtime_error("Invalid socket");
		}

		size_t sent;
		Socket sock(m_socket);


		bool ret = true;
		while (ret) {
			if (m_outputData.size() == 0) {
				return;
			}

			ret = sock.sendNonblock(reinterpret_cast<uint8_t*>(m_outputData.data()), m_outputData.size(), sent);
			std::cout << "Send: ret=" << ret << " sent=" << sent << std::endl;
			if (ret) {
				if (!sent) {
					std::cout << "#Disconnected (write)" << std::endl;
					sock.disconnect();
					break;
					//throw std::runtime_error("Disconnected!");
				}
				m_outputData.erase(m_outputData.begin(), m_outputData.begin() + sent);
                if (m_outputData.size() == 0) {
                    m_ballance = 0;
                }
			}
		}
	}

	bool PollingBuffer::checkForMessages() {
		uint32_t msgSize;
		if (m_inputData.size() >= 4) {
			msgSize = *(reinterpret_cast<uint32_t *>(m_inputData.data()));
            if (m_inputData.size() >= (sizeof(uint32_t) + msgSize)) {
                m_ballance = 1;
                return true;
            } else {
                return false;
            }
        } else {
			return false;
		}

	}

	void PollingBuffer::popMessages() {

		while (checkForMessages()) {
			uint32_t msgSize;
			msgSize = *(reinterpret_cast<uint32_t *>(m_inputData.data()));
			std::string msg(m_inputData.begin() + sizeof(uint32_t),
							m_inputData.begin() + sizeof(uint32_t) + msgSize);
			m_inputData.erase(m_inputData.begin(),
							  m_inputData.begin() + (msgSize + sizeof(uint32_t)));
			m_inbox.push_back(msg);
		}

	}

	void PollingBuffer::pushMessages() {
		while (m_outbox.size() > 0) {
			std::string msg;
			msg = m_outbox.front();
			m_outbox.pop_front();

			uint32_t msgSize = msg.size();
			char *msgSizePtr = reinterpret_cast<char *>(&msgSize);
			m_outputData.insert(m_outputData.end(),
								msgSizePtr, msgSizePtr + sizeof(uint32_t));
			m_outputData.insert(m_outputData.end(), msg.begin(), msg.end());
		}
	}

	bool PollingBuffer::recvMessage(std::string &str) {
		if (m_inbox.size() > 0) {
			str = m_inbox.front();
			m_inbox.pop_front();
			return true;
		} else {
			return false;
		}
	}

	void PollingBuffer::sendMessage(const std::string &msg) {
		m_outbox.push_back(msg);
	}

}
