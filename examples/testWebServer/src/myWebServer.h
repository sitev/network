#pragma once

#include "cj.h"
#include "cjNetwork.h"

#define cnt 10000

namespace cj {
	class MyWebServerHandler : public AbstractWebServerHandler {
	protected:
	public:
		MyWebServerHandler(WebServer *webServer);
		virtual void threadStep(Socket *socket);
		virtual void step(HttpRequest &request, HttpResponse &response);
		virtual void parseRequest(char *req, int len, string &s);
	};

	class MyWebServer : public WebServer {
		//MyWebServerHandler* arr[cnt];
		//int iarr = 0;
	public:
		map<string, int> keys;
		MyWebServer(int port = 80);
		virtual void threadFunction(Socket *socket);
	};
}