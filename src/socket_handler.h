#pragma once

namespace network {

	class SocketHandler {
	public:
		SocketHandler();

		virtual bool wait(ulong &events, SOCKET &sock) = 0;
		virtual bool add(SOCKET sock) = 0;
		virtual void del(SOCKET sock, bool isClose = false) = 0;

	};

}