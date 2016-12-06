#include "myWebServer.h"

namespace cj {

	MyWebServerHandler::MyWebServerHandler(WebServer *webServer) : WebServerHandler(webServer) {
	}

	void MyWebServerHandler::step(HttpRequest &request, HttpResponse &response) {
		MyWebServer *mws = (MyWebServer*)webServer;
		string a = request.header.GET.getValue_s("a");
		int v = mws->keys[a];
//		if (v == 0)
//			keys.insert(std::pair<string, int>(a, 0));
		v++;
		mws->keys[a] = v;

		int rnd = rand() % 100;
		string s = to_string(v) + " " + to_string(rnd);
		s = "HTTP/1.1 200 OK\r\nContent-Length: " + to_string(s.length()) + "\r\n\r\n" + s + "\r\n";
		response.memory.write((void*)(s.c_str()), s.length());
	}

	MyWebServer::MyWebServer(int port) : WebServer(port) {
		srand(123);
		rand();
		srand(321);
	}

	void MyWebServer::threadFunction(Socket *socket) {
		g_mutex.lock();
		MyWebServerHandler *handler = new MyWebServerHandler(this);
		g_mutex.unlock();

		handler->threadStep(socket);

		g_mutex.lock();
		delete handler;
		g_mutex.unlock();
	}
}