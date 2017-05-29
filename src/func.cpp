#include "network.h"

#include <ws2tcpip.h>

namespace network {
	Str getIpByHost(Str host) {
		int status;
		struct addrinfo hints;
		struct addrinfo *servinfo;  // указатель на результаты

		memset(&hints, 0, sizeof hints); // убедимся, что структура пуста
		hints.ai_family = AF_UNSPEC;     // неважно, IPv4 или IPv6
		hints.ai_socktype = SOCK_STREAM; // TCP stream-sockets
		hints.ai_flags = AI_PASSIVE;     // заполните мой IP-адрес за меня

		if ((status = getaddrinfo(host.to_string().c_str(), "80", &hints, &servinfo)) != 0) {
			fprintf(stderr, "getaddrinfo error: %sn", gai_strerror(status));
			exit(1);
		}

		struct in_addr addr;
		addr.S_un = ((struct sockaddr_in *)(servinfo->ai_addr))->sin_addr.S_un;
		string rez = inet_ntoa(addr);
		freeaddrinfo(servinfo); // и освобождаем связанный список

		return rez;
	}
}