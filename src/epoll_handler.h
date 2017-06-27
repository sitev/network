#pragma once

#ifdef OS_LINUX

#define EPOLL_SIZE 10000
#define EPOLL_MAX_EVENTS 1

#define FD_READ_BIT      0
#define FD_READ          (1 << FD_READ_BIT)

#define FD_WRITE_BIT     1
#define FD_WRITE         (1 << FD_WRITE_BIT)

#define FD_OOB_BIT       2
#define FD_OOB           (1 << FD_OOB_BIT)

#define FD_ACCEPT_BIT    3
#define FD_ACCEPT        (1 << FD_ACCEPT_BIT)

#define FD_CONNECT_BIT   4
#define FD_CONNECT       (1 << FD_CONNECT_BIT)

#define FD_CLOSE_BIT     5
#define FD_CLOSE         (1 << FD_CLOSE_BIT)


namespace network {
	class EpollHandler : public SocketHandler {
	protected:
		int epfd;
		struct epoll_event evlist[MAX_EVENTS];
	public:
		EpollHandler();
		virtual bool add(SOCKET sock);
		virtual void del(SOCKET sock, bool isClose = false);
		virtual bool wait(ulong &events, SOCKET &sock);
	};
}

#endif