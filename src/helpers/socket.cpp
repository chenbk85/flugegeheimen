#include "../stdafx.h"
#include "socket.h"

#define DEFAULT_NET_BUF ((size_t)(1024 * 10))
#define DEFAULT_BACKLOG_LEN 20

namespace Flug {

	Socket::Socket() :
			m_sock(-1) {
	}

	Socket::Socket(int sock) :
			m_sock(sock) {
	}

	Socket::Socket(const Socket &oth) :
			m_sock(oth.m_sock) {
	}

	Socket::~Socket() {
	}

	Socket & Socket::operator= (int sockfd) {
		m_sock = sockfd;
		return *this;
	}

	Socket & Socket::operator= (const Socket & sock) {
		m_sock = sock.m_sock;
		return *this;
	}

	void Socket::setNonblocking () {
		if (m_sock == -1) {
			throw std::runtime_error ("Trying to set nonblocking an invalid socket");
		}

		int ret = fcntl(m_sock, F_SETFL, O_NONBLOCK);
		if (ret == -1) {
			throw std::runtime_error ("Setting to nonblock state failed");
		}
	}

	void Socket::connect(const std::string &addr, const std::string &port) {
		addrinfo hints, *results;

		memset(&hints, 0, sizeof(addrinfo));
		hints.ai_family = AF_INET;    /* Allow IPv4 or IPv6 */
		hints.ai_socktype = SOCK_STREAM; /* Datagram socket */

		int ret = getaddrinfo(addr.c_str(), port.c_str(), &hints, &results);
		if (ret) {
			throw std::runtime_error(("Can't getaddrinfo(); of " + addr + ":" + port).c_str());
		}
		int sfd = -1;
		bool connected = false;
		for (addrinfo *rp = results; rp != NULL; rp = rp->ai_next) {
			sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
			if (sfd == -1)
				continue;
			if (::connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
				connected = true;
				break;                  /* Success */
		}
		if (sfd == -1 || connected == false) {
			throw std::runtime_error(("Can't connect to " + addr + ":" + port).c_str());
		} else {
			m_sock = sfd;
		}
	}

	void Socket::bind(const std::string &port) {
		addrinfo hints, *results;

		memset(&hints, 0, sizeof(addrinfo));
		hints.ai_family = AF_INET;    /* Allow IPv4 or IPv6 */
		hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
		hints.ai_flags = AI_PASSIVE;

		int ret = getaddrinfo(nullptr, port.c_str(), &hints, &results);
		if (ret) {
			throw std::runtime_error(("Can't getaddrinfo(); port: " + port).c_str());
		}
		int sfd = -1;
		for (addrinfo *rp = results; rp != NULL; rp = rp->ai_next) {
			sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
			if (sfd == -1)
				continue;
			if (::bind(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
				break;                  /* Success */
		}
		if (sfd == -1) {
			throw std::runtime_error(("Can't bind port: " + port).c_str());
		} else {
			m_sock = sfd;
		}
	}


	void Socket::listen() {
		if (m_sock == -1) {
			throw std::runtime_error("Socket descriptor is not valid");
		}
		int ret = ::listen(m_sock, DEFAULT_BACKLOG_LEN);
		if (ret == -1) {
			throw std::runtime_error("Can't listen");
		}
	}

	int Socket::accept() {
		if (m_sock == -1) {
			throw std::runtime_error("Socket descriptor is not valid");
		}
		int ret = ::accept(m_sock, nullptr, nullptr);
		if (ret == -1) {
			throw std::runtime_error("Can't accept teh connection");
		}
		return ret;
	}

	void Socket::disconnect() {
		if (m_sock != -1) {
			::close(m_sock);
			m_sock = -1;
		}
	}

	size_t Socket::send(const uint8_t *data, size_t size) {
		if (m_sock == -1) {
			throw std::runtime_error("Failed to send: not connected");
		}

		int ret = ::send(m_sock, data, size, 0);
		if (ret == -1) {
			throw std::runtime_error("Failed to send: send(); returned -1");
		}
		return (size_t) ret;
	}

	size_t Socket::recv(uint8_t *data, size_t size) {
		if (m_sock == -1) {
			throw std::runtime_error("Failed to recv: not connected");
		}

		int ret = ::recv(m_sock, data, size, 0);
		if (ret == -1) {
			throw std::runtime_error("Failed to send: recv(); returned -1");
		}
		return (size_t) ret;
	}


	void LogBinarySocket (const uint8_t * data, size_t size) {
		printf("%4lu: ", size);
		for (size_t i = 0; i < size; i++) {
			printf("0x%02x ", data[i]);
		}
		std::cout << std::endl;
	}


	void Socket::sendData (const uint8_t* data, size_t size) {
		size_t chunk = 0;
		//std::cout << "<--";
		//LogBinarySocket(data, size);
		for (size_t sent = 0; sent < size; sent += chunk) {
			chunk = send(data + sent, size - sent);
		}
	}

	void Socket::recvData (uint8_t* data, size_t size) {
		size_t chunk = 0;
		for (size_t sent = 0; sent < size; sent += chunk) {
			chunk = recv(data + sent, size - sent);
		}
		//std::cout << "-->";
		//LogBinarySocket(data, size);
	}

	void Socket::sendString (const std::string & str) {
		if (m_sock == -1) {
			throw std::runtime_error ("Invalid socket");
		}

		uint32_t strLen = str.size();
		sendData(reinterpret_cast<const uint8_t*>(&strLen), sizeof(strLen));
		sendData(reinterpret_cast<const uint8_t*>(str.c_str()), (size_t)strLen);
	}

	void Socket::recvString (std::string & str) {
		if (m_sock == -1) {
			throw std::runtime_error ("Invalid socket");
		}
		uint32_t strLen;
		recvData (reinterpret_cast<uint8_t*>(&strLen), sizeof(strLen));
		std::vector<char> tmpStore((size_t)strLen);
		recvData (reinterpret_cast<uint8_t*>(tmpStore.data()), (size_t)strLen);
		str.assign(tmpStore.begin(), tmpStore.end());
	}

	void Socket::sendLine(const std::string &str) {
		sendData(reinterpret_cast<const uint8_t*>((str + "\n").c_str()), str.length() + 1);
	}

	void Socket::recvLine(std::string &str) {
		char buf[DEFAULT_NET_BUF];
		bool newlineOcured = false;
		str.clear();

		while (!newlineOcured) {
			recv(reinterpret_cast<uint8_t*>(buf), DEFAULT_NET_BUF);
			char * ptr;
			if ((ptr = strstr(buf, "\n")) != NULL) {
				newlineOcured = true;
				str.insert(str.end(), buf, ptr);
			} else {
				str.insert(str.end(), buf, buf + DEFAULT_NET_BUF);
			}
		}
	}


	Socket::operator int() const {
		return m_sock;
	}

	bool Socket::recvNonblock(uint8_t *data, size_t size, size_t &recvd) {
		ssize_t ret = ::recv(m_sock, data, size, 0);
		if (ret == -1) {
			recvd = 0;
			if (errno == EWOULDBLOCK) {
				return false;
			}
            std::cout << "[RECV ERROR ERRNO: " << errno << "]" << std::endl;
            if (errno == ECONNRESET) {
                return true;
            }
			throw std::runtime_error("::recvNonblock() failed");
		}
		recvd = (size_t) ret;
		return true;
	}

	bool Socket::sendNonblock(const uint8_t *data, size_t size, size_t &sent) {

		ssize_t ret = ::send(m_sock, data, size, 0);
		if (ret == -1) {
			sent = 0;
			if (errno == EWOULDBLOCK) {
				return false;
			}
            std::cout << "[SEND ERROR ERRNO: " << errno << "]" << std::endl;
			throw std::runtime_error("::sendNonblock() failed");
		}
		sent = (size_t) ret;
		return true;
	}

}
