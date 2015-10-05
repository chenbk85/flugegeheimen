#pragma once

namespace Flug {

	class Socket {
	public:
		Socket();

		Socket(int sock);

		Socket(const Socket &sock);

		virtual ~Socket();

		operator int () const;

		Socket & operator= (int sockfd);

		Socket & operator= (const Socket & sock);

		void setNonblocking ();

		void connect(const std::string &addr, const std::string &port);

		void bind(const std::string &port);

		void listen();

		int accept();

		void disconnect();

		size_t send(const char *data, size_t size);

		size_t recv(char *data, size_t size);

		void sendData (const char *data, size_t size);

		void recvData (char * data, size_t size);

		void sendString (const std::string & str);

		void recvString (std::string & str);

		void sendLine (const std::string & str);
		void recvLine (std::string & str);

		bool sendNonblock (const char * data, size_t size, size_t & sent);

		bool recvNonblock (char * data, size_t size, size_t & recvd);

	protected:
		int m_sock;
	private:
	};


}
