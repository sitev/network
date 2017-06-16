#pragma once

#ifdef OS_WINDOWS

#include "socket.h"

#define PORT 5150
#define DATA_BUFSIZE 8192
#define EVENT_COUNT 1024 //WSA_MAXIMUM_WAIT_EVENTS

namespace network {

	class WSASocketHandler : public SocketHandler {
		DWORD EventTotal = 0;
		WSAEVENT EventArray[EVENT_COUNT];
		SOCKET SocketArray[EVENT_COUNT];
	public:
		WSASocketHandler();
		~WSASocketHandler();

		bool add(SOCKET sock);
		void del(DWORD Event, bool isClose = false);
		void delSock(SOCKET sock, bool isClose = false);
		uint waitAll(int nCount, HANDLE *lpHandles, int dwSeconds);
		bool wait(DWORD &events, SOCKET &sock);
	};

}

#endif
