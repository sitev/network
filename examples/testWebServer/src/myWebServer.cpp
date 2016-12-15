#define _CRT_SECURE_NO_WARNINGS

#include "myWebServer.h"

namespace cj {

	MyWebServerHandler::MyWebServerHandler(WebServer *webServer) : AbstractWebServerHandler(webServer) {
	}

	void MyWebServerHandler::threadStep(Socket *socket) {
		try {
			int m_sock = socket->m_sock;
			char req[4096];
			int ireq = 0;
			while (true) {
				char buf[1024];
				int len = ::recv(m_sock, buf, 1024, 0);
				if (len <= 0) break;

				memcpy((void*)&req[ireq], buf, len);
				ireq += len;
			}

			string a;
			parseRequest(req, ireq, a);

			MyWebServer *mws = (MyWebServer*)webServer;
			int v = mws->keys[a];
			v++;
			mws->keys[a] = v;

			int rnd = rand();
			char s0[101];
			strcpy(s0, (to_string(v) + " " + to_string(rnd)).c_str());
			int len0 = strlen(s0);
			char sl[10];
			_itoa_s(len0, sl, 10);
			int lenl = strlen(sl);
			//s = "HTTP/1.1 200 OK\r\nContent-Length: " + to_string(s.length()) + "\r\n\r\n" + s + "\r\n";
			//response.memory.write((void*)(s.c_str()), s.length());
			char* s1 = "HTTP/1.1 200 OK\r\nContent-Length: ";
			int len1 = strlen(s1);
			char* s2 = "\r\n\r\n";
			int len2 = strlen(s2);
			char* s3 = "\r\n";
			int len3 = strlen(s3);
			char buf[1024];
			char *buff = buf;
			int i = 0;
			memcpy((void*)buf, s1, len1); i += len1;
			memcpy((void*)&buf[i], sl, len0);   i += len0;
			memcpy((void*)&buf[i], s2, len2); i += len2;
			memcpy((void*)&buf[i], s0, len0);   i += len0;
			memcpy((void*)&buf[i], s3, len3); i += len3;

			
			//socket->sendAll(response.memory);
			socket->send(buf, i);

			socket->close();
			delete socket;
		}
		catch (...) {
			LOGGER_ERROR("Error in threadStep try catch");
		}
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

	void MyWebServerHandler::parseRequest(char *req, int len, string &s) {
		s = "";
		int ireq = 0;
		int pos1, pos2;
		int mode = 1;
		for (int i = 0; i < len; i++) {
			char ch = req[ireq]; ireq++;
			if (mode == 1) {
				if (ch == ' ') mode = 2;
			}
			else if (mode == 2) {
				//ch = req[ireq]; ireq++;
				if (ch == '/') ch = req[ireq]; ireq++;
				if (ch != '?') return;
				ch = req[ireq]; ireq++;
				if (ch != 'a') return;
				ch = req[ireq]; ireq++;
				if (ch != '=') return;
				mode = 3;
			}
			else if (mode == 3) {
				if (ch == ' ') return;
				s += ch;
			}
		}
	}



	MyWebServer::MyWebServer(int port) : WebServer(port) {
		srand(time(NULL));
		//for (int i = 0; i < cnt; i++) {
		//	arr[i] = new MyWebServerHandler(this);
		//}
	}

	void MyWebServer::threadFunction(Socket *socket) {
//		g_mutex.lock();
		MyWebServerHandler *handler = new MyWebServerHandler(this);
		//MyWebServerHandler *handler = arr[iarr];
		//iarr++;
		//if (iarr >= cnt) iarr = 0;
//		g_mutex.unlock();

		handler->threadStep(socket);

//		g_mutex.lock();
		delete handler;
//		g_mutex.unlock();
	}
}