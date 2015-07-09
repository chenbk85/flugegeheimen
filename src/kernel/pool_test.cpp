#include "../stdafx.h"
#include "pool.h"
#include "../helpers/socket.h"
#include "gtest/gtest.h"

void ConnectorThreadProc (const std::string & addr, const std::string & port) {
	Flug::Socket sock;
	EXPECT_NO_THROW(sock.connect (addr, port));
	sock.disconnect();
}

void SenderThreadProc (const std::string & addr, const std::string & port) {
	Flug::Socket sock;
	EXPECT_NO_THROW(sock.connect (addr, port));
	EXPECT_NO_THROW(sock.send("abcd", 4));
	sock.disconnect();
}

TEST (Pool, TestBareEpoll) {
	Flug::Socket listner;
	listner.bind("6345");
	listner.setNonblocking();
	listner.listen();


	int epoll = epoll_create(1);
	EXPECT_NE(epoll, -1);
	epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = (int)listner;
	int ret = epoll_ctl(epoll, EPOLL_CTL_ADD, (int)listner, &ev);
	EXPECT_NE (ret, -1);


	epoll_event events[10];
	std::thread thr(ConnectorThreadProc, "localhost", "6345");
	epoll_wait (epoll, events, 10, 2000);
	listner.accept();
	listner.disconnect();
	EXPECT_EQ(1, 1);
	thr.join();
}

TEST (Pool, TestListen) {
}


