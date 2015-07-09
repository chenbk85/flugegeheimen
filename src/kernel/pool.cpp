#include "../stdafx.h"
#include "pool.h"


namespace Flug {


	Pool::Pool () 
	: m_sinkSock(-1) {
		m_poll = epoll_create (1);
		if (m_poll == -1) {
			throw std::runtime_error ("Can't create an epoll instance");
		}
	}

	Pool::~Pool () {
	}

	Pool & Pool::getInstance () {
		static Pool pool;
		return pool;
	}

	/*void Pool::addGatewaySocket (const Socket & socket) {
	  if (m_poll == -1) {
	  throw std::runtime_error ("Epoll instance does not exist");
	  }

	  if ((int)socket == -1) {
	  throw std::runtime_error ("Trying to add to epoll an invalid socket");
	  }

	  epoll_event ev;
	  ev.events = EPOLLIN;
	  ev.data.fd = (int) socket;
	  int err = epoll_ctl (m_poll, EPOLL_CTL_ADD, (int)socket, &ev);
	  if (err) {
	  throw std::runtime_error ("Failed to add socket to epoll instance");		
	  }
	  }*/

	void Pool::addSinkSocket (const Socket & sink) {
		if (m_poll == -1) {
			throw std::runtime_error ("Epoll instance does not exist");
		}

		if ((int)sink == -1) {
			throw std::runtime_error ("Trying to add to epoll an invalid socket");
		}

		m_sinkEventMask.events = EPOLLIN | EPOLLOUT | EPOLLET;
		m_sinkEventMask.data.fd = (int)sink;
		m_sinkSock = (int)sink;

		int err = epoll_ctl (m_poll, EPOLL_CTL_ADD, (int)sink, &m_sinkEventMask);
		if (err) {
			throw std::runtime_error ("Can't add sink to polling pool");
		}
	}

	void Pool::updateSinkSocket (const Socket & sock) {
		int err = epoll_ctl (m_poll, EPOLL_CTL_DEL, m_sinkSock, NULL);
		if (err) {
			throw std::runtime_error("Can't remove previous sink from poll");
		}
		m_sinkSock = (int)sock;
		m_sinkEventMask.data.fd = (int)sock;
		err = epoll_ctl (m_poll, EPOLL_CTL_ADD, m_sinkSock, &m_sinkEventMask);
		if (err) {
			throw std::runtime_error("Can't add new sink to poll");
		}
	}

	void Pool::removeSinkSocket () {
		int err = epoll_ctl (m_poll, EPOLL_CTL_DEL, m_sinkSock, NULL);
		if (err) {
			throw std::runtime_error("Can't remove previous sink from poll");
		}
		m_sinkSock = -1;
		m_sinkEventMask.data.fd = -1;
	}

	void Pool::wait () {
		epoll_event events[10];
		int ret = epoll_wait(m_poll, events, 10, 100);
		if (ret == -1) {
			throw std::runtime_error ("Error while waiting for epoll events");
		}

		if (ret == 0) {
			throw std::runtime_error ("No events");
		}

		if (ret >= 1) {
			throw std::runtime_error("Lol, it works!");
		}
	}



}
