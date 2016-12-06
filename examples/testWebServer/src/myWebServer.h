#pragma once

#include "cj.h"
#include "cjNetwork.h"

namespace cj {
	class MyWebServerHandler : public WebServerHandler {
	protected:
	public:
		MyWebServerHandler(WebServer *webServer);
		virtual void step(HttpRequest &request, HttpResponse &response);
	};

	class MyWebServer : public WebServer {
	public:
		map<string, int> keys;
		MyWebServer(int port = 80);
		virtual void threadFunction(Socket *socket);
	};
}