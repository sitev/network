#include "core.h"

#ifdef OS_WINDOWS

#include <stdio.h>
#include <time.h>
#include "func.h"

#include "network.h"

namespace network {

	WSASocketHandler::WSASocketHandler() {
	}


	WSASocketHandler::~WSASocketHandler() {
	}

	bool WSASocketHandler::add(SOCKET sock) {
		if ((EventArray[EventTotal] = WSACreateEvent()) == WSA_INVALID_EVENT) {
			//printf("WSACreateEvent() failed with error %d\n", WSAGetLastError());
			return false;
		}

		SocketArray[EventTotal] = sock;

		WSAEventSelect(sock, EventArray[EventTotal], FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE /*FD_ALL_EVENTS*/);
		EventTotal++;

		//printf("SocketHandler.add: sock = %d, total = %d\n", sock, EventTotal);

		return true;
	}

	void WSASocketHandler::del(DWORD Event, bool isClose) {
		SOCKET sock = SocketArray[Event];
		//printf("SocketHandler.del: sock = %d, total = %d\n", sock, EventTotal - 1);
		if (isClose) closesocket(sock);

		if (!WSACloseEvent(EventArray[Event]))
			printf("WSACloseEvent() failed miserably!\n");

		// Squash the socket and event arrays
		for (int i = Event; i < EventTotal; i++)
		{
			EventArray[i] = EventArray[i + 1];
			SocketArray[i] = SocketArray[i + 1];
		}
		EventTotal--;
		//printf("----- del sock = %d, total = %d\n", sock, EventTotal);
	}

	void WSASocketHandler::delSock(SOCKET sock, bool isClose) {
		//printf("----- delSock = %d\n", sock);
		for (int i = 0; i < EventTotal; i++) {
			if (SocketArray[i] == sock) {
				del(i, isClose);
				return;
			}
		}
		int a = 1;
	}

	uint WSASocketHandler::waitAll(int count, HANDLE *lpHandles, int dwSeconds) {
		while (true) {
			uint ret = WSA_WAIT_TIMEOUT;
			for (int i = 0; i < count; i += WSA_MAXIMUM_WAIT_EVENTS) {
				int cnt = min(WSA_MAXIMUM_WAIT_EVENTS, count - i);
				ret = WSAWaitForMultipleEvents(cnt, lpHandles + i, false, 0, 0);
				if (ret < WSA_MAXIMUM_WAIT_EVENTS) {
					return ret + i;
				}
			}
			usleep(10000);
			//printf("+");
			break;
		}
		return WSA_WAIT_FAILED;
	}

	bool WSASocketHandler::wait(DWORD &events, SOCKET &sock) {
		//	printf("WSAWaitForMultipleEvents() is waiting... EventTotal = %d\n", EventTotal);

			// Wait for one of the sockets to receive I/O notification and
			//DWORD Event = WSAWaitForMultipleEvents(EventTotal, EventArray, false, WSA_INFINITE, FALSE);
		uint Event = waitAll(EventTotal, EventArray, 50);
		if (Event == WSA_WAIT_FAILED) {
			return false;
			uint Event = waitAll(EventTotal, EventArray, 50);
			printf("WSAWaitForMultipleEvents() failed with error %d\n", WSAGetLastError());
			return false;
		}
		//	printf("WSAWaitForMultipleEvents() is OK!\n");

		WSANETWORKEVENTS NetworkEvents;
		if (WSAEnumNetworkEvents(SocketArray[Event], EventArray[Event], &NetworkEvents) == SOCKET_ERROR) {
			printf("sock = %d\n", SocketArray[Event]);
			printf("WSAEnumNetworkEvents() is ERROR %d\n", WSAGetLastError());
			return false;
		}
		sock = SocketArray[Event];
		//	printf("WSAEnumNetworkEvents() is OK! socket = %d\n", sock);

		if (NetworkEvents.lNetworkEvents & FD_READ && NetworkEvents.iErrorCode[FD_READ_BIT] != 0) {
			printf("FD_READ failed with error %d\n", NetworkEvents.iErrorCode[FD_READ_BIT]);
			return false;
		}
		//	printf("FD_READ is OK!\n");

		if (NetworkEvents.lNetworkEvents & FD_WRITE && NetworkEvents.iErrorCode[FD_WRITE_BIT] != 0) {
			printf("FD_WRITE failed with error %d\n", NetworkEvents.iErrorCode[FD_WRITE_BIT]);
			return false;
		}
		//	printf("FD_WRITE is OK!\n");

		events = NetworkEvents.lNetworkEvents;
		///printf("EVENTS = %d, SOCKET = %d\n", events, sock);

		return true;
	}

}

#endif
