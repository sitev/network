#pragma once

#include "cj.h"

//#include <sys/epoll.h>

namespace cj {

enum ParamType {ptGET, ptPOST, ptCOOKIE};

class RequestHeader : public ParamList {
public:
	ParamList GET;
	ParamList POST;
	ParamList COOKIE;
	bool isFileFlag;
	string fileExt;
	RequestHeader();
	virtual bool parse(Memory &request);
	virtual void parsePOSTParams(Memory &memory);
private:
	virtual bool parseParams(String sParams, ParamType pt);
	virtual bool isFile(string s, string &fileExt);
	virtual string urlDecode(string s);
	virtual int find(Memory &request, char a);
	virtual int find(Memory &request, string s);
	virtual string substr(Memory &request, int pos, int count);
	virtual char getChar(Memory &request, int pos);
	virtual string decodeHtmlTags(string s, string tag, string dst);
public:
	virtual string htmlEntities(string s);
	virtual string htmlEntitiesDecode(string s);
};

class HttpRequest {
public:
	Memory memory;
	RequestHeader header;
	HttpRequest() {}
	virtual ~HttpRequest() {}

	virtual void parse();
};

class HttpResponse {
public:
	Memory memory;
	HttpResponse() {
	}

};

class WebServerHandler : public Object {
protected:
	HttpRequest request;
	HttpResponse response;
public:
	WebServerHandler() {}
private:
	virtual void recvMemory(Socket *socket, Memory &memory);
	virtual bool check2CRLF(Memory &memory);
public:
	virtual void threadStep(Socket *socket);
	virtual void internalStep(HttpRequest &request, HttpResponse &response);
	virtual void step(HttpRequest &request, HttpResponse &response);
	virtual bool isPageExist(string host) { return false; }
};

class WebServer : public Application {
protected:
	WebServerHandler *handler;
public:

	int socketPort;
	ServerSocket *ss;
//	int epoll_fd;
//	struct epoll_event event;
//	struct epoll_event *events;

	WebServer(int port = 80);
	virtual ~WebServer() {}
	virtual void run();
	virtual void setHandler(WebServerHandler *handler);
	virtual void threadStep(Socket *socket);
	virtual void threadFunction(Socket *socket);
};

}
