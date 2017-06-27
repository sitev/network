#include "core.h"

#ifdef OS_LINUX

#include "network.h"

namespace network {

	EpollHandler::EpollHandler() {
		epfd = epoll_create(5);
	}

	bool EpollHandler::add(SOCKET sock) {
		event.data.fd = sock;
		ev.events = EPOLLIN;

		int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &event);
		return ret == 0;
	}

	bool EpollHandler::del(SOCKET sock, bool isClose = false) {
		event.data.fd = sock;
		event.events = EPOLLIN;

		int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, sock, &event);

		if (isClose) close(sock); //≈сли нужно закрыть сокет - закроем его

		return ret == 0;
	}

	bool EpollHandler::wait(ulong &events, SOCKET &sock) {
		int ret = epoll_wait(epfd, evlist, MAX_EVENTS, -1);
		if (ret == -1) return false;
		for (int i = 0; i < ret; i++) {
		}
	}
}

#endif
