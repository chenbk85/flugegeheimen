#include "../stdafx.h"
#include "pool.h"


namespace Flug {


	ConnectionPool::ConnectionPool() {
		m_poll = epoll_create (1);
		if (m_poll == -1) {
			throw std::runtime_error ("Can't create an epoll instance");
		}
	}

	ConnectionPool::~ConnectionPool() {
		if (m_poll != -1) {
			close(m_poll);
		}
	}



	void ConnectionPool::addSocket(const Socket &sink, uint32_t evmask) {
		if (m_poll == -1) {
			throw std::runtime_error ("Epoll instance does not exist");
		}

		if ((int)sink == -1) {
			throw std::runtime_error ("Trying to add to epoll an invalid socket");
		}

		epoll_event ev;
		PollingBuffer *pbuf = new PollingBuffer((int)sink);
		m_buffers.push_back(pbuf);

		ev.events = evmask;
		ev.data.ptr = pbuf;

		int err = epoll_ctl (m_poll, EPOLL_CTL_ADD, (int)sink, &ev);
		if (err) {
			throw std::runtime_error ("Can't add sink to polling pool");
		}
	}

	void ConnectionPool::updateSocket(const Socket &sock, uint32_t evmask) {
		if (m_poll == -1) {
			throw std::runtime_error ("Epoll instance does not exist");
		}

		epoll_event ev;
		ev.data.fd = (int)sock;
		ev.events = evmask;
		int err = epoll_ctl (m_poll, EPOLL_CTL_MOD, (int)sock, &ev);
		if (err) {
			throw std::runtime_error("Can't update sink in poll");
		}
	}

	void ConnectionPool::removeSocket (const Socket & sock) {
		if (m_poll == -1) {
			throw std::runtime_error ("Epoll instance does not exist");
		}

		int err = epoll_ctl (m_poll, EPOLL_CTL_DEL, (int)sock, NULL);
		if (err) {
			throw std::runtime_error("Can't remove sink from poll");
		}
	}

	size_t ConnectionPool::wait (epoll_event * evs, size_t maxEvs) {
		int ret = epoll_wait(m_poll, evs, maxEvs, 100);
		if (ret == -1) {
			throw std::runtime_error ("Error while waiting for epoll events");
		}

		return ret;
	}



}
