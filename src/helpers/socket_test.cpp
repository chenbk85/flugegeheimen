#include "../stdafx.h"
#include "socket.h"
#include "gtest/gtest.h"



namespace Flug {
	static void generateRandomData (char * buf, size_t size) {
		srand(time(NULL));
		for (size_t s = 0; s < size; s++) {
			buf[s] = rand();
		}
	}

	static void connectorThread(const std::string &host, const std::string &port) {
		Socket sock;
		EXPECT_NO_THROW(sock.connect(host, port));
		sock.disconnect();
	}

	static void senderThread(const std::string &host, const std::string &port, const char * data, size_t size) {
		Socket sock;
		sock.connect(host, port);
		sock.send(reinterpret_cast<const uint8_t*>(data), size);
		sock.disconnect();
	}

	TEST (Socket, Connect) {
		Socket sock;
		sock.connect("google.com", "http");
		sock.disconnect();
	}

	TEST (Socket, TestBind) {
		Socket sock;
		EXPECT_NO_THROW(sock.bind("3456"));
		EXPECT_NO_THROW(sock.listen());
		std::thread thrd(connectorThread, "localhost", "3456");
		thrd.join();
		sock.disconnect();
	}

	TEST (Socket, TestSendRecv) {
		Socket sock;
		char buf[16], rbuf[16];
		generateRandomData(buf, 16);
		std::thread thrd(senderThread, "localhost", "3456", buf, 16);
		EXPECT_NO_THROW(sock.bind("3456"));
		EXPECT_NO_THROW(sock.listen());
		int sockfd;
		EXPECT_NO_THROW(sockfd = sock.accept());
		Socket reciever(sockfd);
		EXPECT_NO_THROW(reciever.recv(reinterpret_cast<uint8_t *>(rbuf), 16));
		EXPECT_EQ(memcmp(buf, rbuf, 16), 0);
		thrd.join();
		sock.disconnect();
		reciever.disconnect();
	}

}
