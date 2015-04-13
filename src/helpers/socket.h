#pragma once

namespace Flug {

	class Socket {
	public:
		Socket();

		Socket(int sock);

		Socket(const Socket &sock);

		virtual ~Socket();

		void connect(const std::string &addr, const std::string &port);

		void bind(const std::string &port);

		void listen();

		int accept();

		void disconnect();

		size_t send(const char *data, size_t size);

		size_t recv(char *data, size_t size);

		void sendData (const char *data, size_t size);

		void recvData (char * data, size_t size);

	protected:
		int m_sock;
	private:
	};


}
