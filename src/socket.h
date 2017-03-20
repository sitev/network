#pragma once

#include "cj.h"

namespace cj {

#ifdef OS_LINUX
#define SOCKET int
#endif

const int MAXHOSTNAME = 200;
const int MAXCONNECTIONS = 500;
const int MAXRECV = 4096;// 8192;

class Socket : public Object {
public:
	SOCKET m_sock;
	int ticks = 0;
protected:
	sockaddr_in m_addr; //old model for Windows è Linux
#ifdef OS_WINDOWS
	addrinfo *result = NULL, *ptr = NULL, hints; // new model for IPv6
#endif
	bool fNonBlocking, error, sendRequest;
public:
	Socket();
	Socket(SOCKET sock);
	virtual void init();
	virtual ~Socket();
	virtual bool create();
	virtual bool create(int domain, int type, int protocol);

	virtual int getCurSize();
	virtual int recv(String &s);
	virtual int recv(void *buffer, int size);
	virtual int recv(Memory &memory);
	virtual int Socket::recv_new(Memory &memory);
	virtual int recv(Memory &memory, int size);
	virtual int recv_new(Memory &memory, int size1);

	virtual int send(String s);
	virtual int send(void *buffer, int size);
	virtual int send(Memory &memory);

	static bool sendAll(SOCKET sock, void *buffer, int size);
	virtual bool sendAll(void *buffer, int size);
	virtual bool sendAll(Memory &memory);

	virtual void setNonBlocking(bool);
	virtual bool isValid();
	virtual bool isError();
	virtual void setError(bool error);

	virtual bool getSendRequest();
	virtual void setSendRequest(bool value);

	virtual void close();
};

class ClientSocket : public Socket {
public:
	ClientSocket();
	ClientSocket(SOCKET sock);
	virtual ~ClientSocket();

	virtual bool connect(String host, int port);
};
class ServerSocket1 {
public:
	ServerSocket1();
	ServerSocket1(SOCKET sock);
	virtual ~ServerSocket1();
};
class ServerSocket : public Socket {
public:
	List lstSocket;
	ServerSocket();
	ServerSocket(SOCKET sock);
	virtual ~ServerSocket();

	virtual bool create();
	virtual bool create(int domain, int type, int protocol);
	virtual bool bind(int port);
	virtual bool bind(int port, ushort family, uint addr);
	virtual bool listen();
	virtual bool listen(int connCount);
	virtual bool accept();
	virtual Socket* acceptLight();
	virtual void setNonBlocking(bool);
};

}
