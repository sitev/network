#define _CRT_SECURE_NO_WARNINGS

#include "cj.h"
#include "cjNetwork.h"
#include <sys/stat.h>

//#include <sys/epoll.h>
#include <signal.h>


namespace cj {

//--------------------------------------------------------------------------------------------------
//----------          class RequestHeader          -------------------------------------------------
//--------------------------------------------------------------------------------------------------

RequestHeader::RequestHeader() {

}

bool RequestHeader::parse(Memory &request) {
	isFileFlag = false;
	clear();

	LOGGER_TRACE("request parse");
	int pos1 = find(request, ' ');
	if (pos1 <= 0) return false;

	string method = substr(request, 0, pos1);
	add("Method", method);

	LOGGER_TRACE("method = " + method);

	pos1++;

	int savePos = request.getPos();
	string httpstr = " HTTP/1.1\r\n";
	int pos2 = find(request, httpstr);
	printf("pos1 = %d, pos2 = %d\n", pos1, pos2);
	if (pos2 <= pos1) {
		printf("pos2 <= pos1\n");
		request.setPos(savePos);
		httpstr = " HTTP/1.0\r\n";
		pos2 = find(request, httpstr);
		printf("pos1 = %d, pos2 = %d\n", pos1, pos2);
		if (pos2 <= pos1) return false;
		add("Version", (string)"1.0");
		LOGGER_OUT("Version", "Version = 1.0");

	}
	else {
		add("Version", (string)"1.1");
		LOGGER_OUT("Version", "Version = 1.1");
	}

	char ch = getChar(request, pos1);
	if (ch != '/') return false;
	ch = getChar(request, pos1 + 1);
	if (ch == '?') pos1++;

	string sParams = substr(request, pos1 + 1, pos2 - pos1 - 1);
	//string sDecode = decodeCp1251(sParams);
	string sDecode = sParams;
	add("Params", sDecode);
	LOGGER_OUT("Params", "Params = " + sDecode);


	if (isFile(sParams, fileExt)) {
		isFileFlag = true;
	}
	else 
		parseParams(sParams, ptGET);


	while (true) {
		pos1 = request.getPos();
		pos2 = find(request, ":");
		if (pos2 < 0) {
			request.setPos(pos1);
			int pos3 = find(request, "\r\n");
			break;
		}
		int pos3 = find(request, "\r\n");

		if (pos3 <= 0) break;
		string name = substr(request, pos1, pos2 - pos1);
		string value = substr(request, pos2 + 2, pos3 - pos2 - 2);
		add(name, value);


		if (name == "Cookie") {
			parseParams(value, ptCOOKIE);
		}
	}
	/******
	if (method == "POST") {
		pos1 = pos1 + 2;
		sParams = substr(request, pos1, request.getSize() - pos1);
//		string sDecode = decodeCp1251(sParams);
		string sDecode = sParams;
		parseParams(sDecode, ptPOST);
	}
	*/

	pos1 = request.getPos();
	return true;
}

void RequestHeader::parsePOSTParams(Memory &memory) {
	int pos = memory.getPos();
	string sParams = substr(memory, pos, memory.getSize() - pos);
	string sDecode = sParams;
	parseParams(sDecode, ptPOST);
}



bool RequestHeader::parseParams(String sParams, ParamType pt) {
	ParamList *params;
	if (pt == ptGET) params = &GET;
	else if (pt == ptPOST) 
		params = &POST;
	else 
		params = &COOKIE;

	params->clear();
	if (sParams == "") return true;

	string s = sParams.to_string();

	int pos = 0;
	string tmp = s.substr(pos, 1);
	if (tmp == "/") {
		pos++;
	}

	tmp = s.substr(pos, 1);
	if (tmp == "?") {
		pos++;
	}

	string path = s.substr(pos);

	//string path = "param1/param2/name1=value1&name2=value2";

	pos = 0;
	int posEnd = path.length();
	int mode = 1;
	int index = 1;
	string name = "", value = "";

	while (true) {
		if (pos >= posEnd || path[pos] == '/' || path[pos] == '&' || path[pos] == '?' || path[pos] == ';') {
			if (mode == 1) {
				if (name != "") {
					name = urlDecode(name);
					name = htmlEntities(name);
					value = name;
					name = "p" + to_string(index);
					index++;
					params->insert(name, value);
					name = "";
					value = "";
				}
			}
			else {
				name = urlDecode(name);
				value = urlDecode(value);
				name = htmlEntities(name);
				value = htmlEntities(value);
				params->insert(name, value);
				name = "";
				value = "";
				mode = 1;
			}
			if (pos >= posEnd) break;
		}
		else if (path[pos] == '=') {
			mode = 2;
		}
		else if (path[pos] == ' ') {
			//null
		}
		else {
			if (mode == 1) name += path[pos]; else value += path[pos];
		}
		pos++;
	}

	return true;
}


bool RequestHeader::isFile(string s, string &fileExt) {
	int pos = s.find_last_of('.');
	if (pos < 0) return false;
	fileExt = s.substr(pos + 1);
	return true;
}

string RequestHeader::urlDecode(string src) {
	string ret;
	int len = src.length();
	for (int i = 0; i < len; i++) {
		if (int(src[i]) == 37) {
			int ichar;
			sscanf(src.substr(i + 1, 2).c_str(), "%x", &ichar);
			char ch = static_cast<char>(ichar);
			ret += ch;
			i = i + 2;
		}
		else if (int(src[i]) == 43) {
			ret += ' ';
		}
		else {
			ret += src[i];
		}
	}
	return ret;
}

int RequestHeader::find(Memory &request, char a) {
	while (!request.eof()) {
		char ch;
		request.read(&ch, 1);
		if (ch == a) return request.getPos() - 1;
	}

	return -1;
}

int RequestHeader::find(Memory &request, string s) {
	int index;
	int len = s.length();
	if (len <= 0) return -1;
	bool flag = false;
	while (!request.eof()) {
		if (!flag) {
			flag = true;
			if (find(request, s[0]) < 0) flag = false;
			index = 1;
		}
		else {
			char ch;
			request.read(&ch, 1);
			char a = s[index];
			index++;
			if (ch != a) {
				flag = false;
				index = 0;
			}
		}
		if (len <= index) {
			if (flag) return request.getPos() - len;
		}
	}
	return -1;
}

string RequestHeader::substr(Memory &request, int pos, int count) {
	char *s = (char*)malloc(count + 1);
	memcpy(s, (char*)request.data + pos, count);
	s[count] = 0;
	string ret = s;
	free(s);
	return ret;
}

char RequestHeader::getChar(Memory &request, int pos) {
	return ((char*)request.data)[pos];
}

string RequestHeader::htmlEntities(string s) {
	string r = "";
	int len = s.length();
	for (int i = 0; i < len; i++) {
		char ch = s[i];
		switch (ch) {
		case '&': {
			r = r + "&amp";
			break;
		}
		case '<': {
			r = r + "&lt";
			break;
		}
		case '>': {
			r = r + "&gt";
			break;
		}
		case '"': {
			r = r + "&guot";
			break;
		}
		case '\'': {
			r = r + "&apos";
			break;
		}
		default:
			r = r + ch;
		}
	}
	return r;
}

string RequestHeader::decodeHtmlTags(string s, string tag, string dst) {
	while (true) {
		int pos = s.find(tag);
		if (pos < 0) break;
		s.replace(pos, tag.size(), dst);
	}
	return s;
}

string RequestHeader::htmlEntitiesDecode(string s) {
	s = decodeHtmlTags(s, "&lt!--", "<!--");
	s = decodeHtmlTags(s, "&ltp&gt", "<p>");
	s = decodeHtmlTags(s, "&ltb", "<b");
	s = decodeHtmlTags(s, "&lta", "<a");
	s = decodeHtmlTags(s, "&lti", "<i");
	s = decodeHtmlTags(s, "&ltblockquote&gt", "<blockquote>");
	s = decodeHtmlTags(s, "&ltstrong&gt", "<strong>");
	s = decodeHtmlTags(s, "&ltspan", "<span");
	s = decodeHtmlTags(s, "&ltli&gt", "<li>");
	s = decodeHtmlTags(s, "&lte", "<e");
	s = decodeHtmlTags(s, "&lth", "<h");
	s = decodeHtmlTags(s, "&ltu", "<u");
	s = decodeHtmlTags(s, "&lto", "<o");
	s = decodeHtmlTags(s, "&ltfont", "<font");
	s = decodeHtmlTags(s, "&ltdiv", "<div");
	s = decodeHtmlTags(s, "&ltt", "<t");
	s = decodeHtmlTags(s, "&ltc", "<c");

	s = decodeHtmlTags(s, "&ltbr", "<br");

	s = decodeHtmlTags(s, "-->&gt", "-->");
	s = decodeHtmlTags(s, "&lt/p&gt", "</p>");
	s = decodeHtmlTags(s, "&lt/b", "</b");
	s = decodeHtmlTags(s, "&lt/a", "</a");
	s = decodeHtmlTags(s, "&lt/i", "</i");
	s = decodeHtmlTags(s, "&lt/blockquote&gt", "</blockquote>");
	s = decodeHtmlTags(s, "&lt/strong&gt", "</strong>");
	s = decodeHtmlTags(s, "&lt/span&gt", "</span>");
	s = decodeHtmlTags(s, "&lt/ul&gt", "</ul>");
	s = decodeHtmlTags(s, "&lt/li&gt", "</li>");
	s = decodeHtmlTags(s, "&lt/e", "</e");
	s = decodeHtmlTags(s, "&lt/h", "</h");
	s = decodeHtmlTags(s, "&lt/u", "</u");
	s = decodeHtmlTags(s, "&lt/o", "</o");
	s = decodeHtmlTags(s, "&lt/font", "</font");
	s = decodeHtmlTags(s, "&lt/div", "</div");
	s = decodeHtmlTags(s, "&lt/t", "</t");
	s = decodeHtmlTags(s, "&lt/c", "</c");

	s = decodeHtmlTags(s, "&guot", "\"");
	s = decodeHtmlTags(s, "&gt", ">");
	s = decodeHtmlTags(s, "&ampnbsp;", "&nbsp;");

	return s;
}



//--------------------------------------------------------------------------------------------------
//----------          class HttpRequest          ---------------------------------------------------
//--------------------------------------------------------------------------------------------------

void HttpRequest::parse() {
	LOGGER_TRACE("Start parse");

	memory.setPos(0);
	header.parse(memory);

	LOGGER_TRACE("Finish parse");
}



//--------------------------------------------------------------------------------------------------
//----------          class WebServerHandler          ----------------------------------------------
//--------------------------------------------------------------------------------------------------
void WebServerHandler::recvMemory(Socket *socket, Memory &memory) {
	while (true) {
		int len = socket->recv(memory);
		if (len <= 0) break;
	}
}

bool WebServerHandler::check2CRLF(Memory &memory) {
	int pos = memory.getPos();
	int posEnd = memory.getSize();
	int step = 0;
	while (true) {
		if (pos >= posEnd) 
			return false;

		char u = *(((char*)memory.data) + pos);
		printf("%c", u);
		if (step == 0 && u == '\015')
			step++;
		else if (step == 1 && u == '\012')
			step++;
		else if (step == 2 && u == '\015')
			step++;
		else if (step == 3 && u == '\012') {
			pos++;
			return true;
		}
		pos++;
	}
}

void WebServerHandler::threadStep(Socket *socket) {
	try {
		LOGGER_OUT("MUTEX", "application->g_mutex.lock(); {");
		application->g_mutex.lock();
		LOGGER_OUT("MUTEX", "application->g_mutex.lock(); }");
		request.memory.setPos(0);
		request.memory.setSize(0);
		response.memory.setPos(0);
		response.memory.setSize(0);

		application->g_mutex.unlock();

		while (true) {
			while (true) {
				application->g_mutex.lock();
				recvMemory(socket, request.memory);
				bool flag = check2CRLF(request.memory);
				application->g_mutex.unlock();
				if (flag) break;
				usleep(1000);
			}

			application->g_mutex.lock();

			printf("----------\n");
			string s = "";
			int count = request.memory.getSize();
			for (int i = 0; i < count; i++) {
				printf("%c", ((char*)request.memory.data)[i]);
				s = s + ((char*)request.memory.data)[i];
			}
			printf("----------\n");
			LOGGER_OUT("HTML", s);
			printf("\n");

			request.parse();
			int pos = request.memory.getPos();
			String method = request.header.getValue("Method");
			int conLen = request.header.getValue("Content-Length").toInt();
			application->g_mutex.unlock();

			if (conLen > 0) {
				while (true) {
					application->g_mutex.lock();
					recvMemory(socket, request.memory);
					int size = request.memory.getSize();
					int delta = size - pos;
					application->g_mutex.unlock();
					if (delta >= conLen) break;
					usleep(1000);
				}
				application->g_mutex.lock();
				if (method == "POST")
					request.header.parsePOSTParams(request.memory);
				application->g_mutex.unlock();
			}

			LOGGER_OUT("MUTEX", "application->g_mutex.unlock();");

			internalStep(request, response);

			LOGGER_OUT("MUTEX", "application->g_mutex.lock(); {");
			application->g_mutex.lock();
			LOGGER_OUT("MUTEX", "application->g_mutex.lock(); }");
			if (socket->sendAll(response.memory)) LOGGER_TRACE("sendAll OK!"); else LOGGER_TRACE("sendAll error ...");
			application->g_mutex.unlock();

			break;
		}

		socket->close();
		delete socket;
		//application->g_mutex.unlock();
	}
	catch(...) {
		LOGGER_ERROR("Error in threadStep try catch");
	}
}

void WebServerHandler::internalStep(HttpRequest &request, HttpResponse &response) {
	string host = request.header.getValue("Host").to_string();
	if (host == "127.0.0.1:8080") host = LOCALHOST;

	if (!request.header.isFileFlag)
	{
		bool flagSiteExist = isSiteExist(host);
		bool flagPageExist = isPageExist(host);
		if (!flagSiteExist || !flagPageExist) {
			string fn;
			if (!flagSiteExist) fn = "/var/www/common/404site_tpl.html";
			else fn = "/var/www/common/404page_tpl.html";
			File *f = new File(fn, "rb");
			bool flag = f->isOpen();
			if (flag) {
				string version = request.header.getValue_s("Version");
				printf("version = %s\n", version.c_str());
				string s = "HTTP/" + version + " 404 Not Found\r\nContent-Type: text/html";
				int sz = f->getSize();
				s = s + "\r\nConnection: keep-alive\r\nKeep-Alive: timeout=5, max=100\r\nContent-Length: " + to_string(sz) + "\r\n\r\n";
				response.memory.write((void*)(s.c_str()), s.length());

				Memory mem;
				mem.setSize(sz);
				f->read(mem.data, sz);
				response.memory.write(mem.data, sz);
			}
			delete f;
		}
		else {
			step(request, response);
		}
	}
	else {
		string fn1 = request.header.getValue_s("Params");
		if (fn1 == "") {
			fn1 = "index_tpl.html";
			request.header.fileExt = "html";
		}
		string fn = "/var/www/" + host + "/" + fn1;
		int ret;
		struct stat buf;
		if ((ret = stat(fn.c_str(), &buf)) != 0) {
			fn = "/var/www/common/" + fn1;
		}

		printf("filename = %s\n", fn.c_str());
		File *f = new File(fn, "rb");
		bool flag = f->isOpen();
		if (flag) {
			string version = request.header.getValue_s("Version");
			printf("version = %s\n", version.c_str());
			string s = "HTTP/" + version + " 200 OK\r\nContent-Type: ";
			if (request.header.fileExt == "html") s = s + "text/html";
			else if (request.header.fileExt == "ico") s = s + "image/ico";
			else if (request.header.fileExt == "png") s = s + "image/png";
			else if (request.header.fileExt == "jpg") s = s + "image/jpeg";
			else if (request.header.fileExt == "js") s = s + "text/javascript";
			else if (request.header.fileExt == "css") s = s + "text/css";
			else if (request.header.fileExt == "gif") s = s + "image/gif";
			else if (request.header.fileExt == "apk") s = s + "application/vnd.android.package-archive";
			else if (request.header.fileExt == "jar") s = s + "application/java-archive";
			else if (request.header.fileExt == "jad") s = s + "text/vnd.sun.j2me.app-descriptor";
			else s = s + "application/octet-stream";

			int sz = f->getSize();
			s = s + "\r\nConnection: keep-alive\r\nKeep-Alive: timeout=5, max=100\r\nContent-Length: " + to_string(sz) + "\r\n\r\n";
			//s = s + "\r\nConnection: keep-alive\r\nKeep-Alive: timeout=5, max=100\r\nSet-Cookie: name=newvalue\r\nContent-Length: " + to_string(sz) + "\r\n\r\n";

			LOGGER_OUT("HTTP", s);

			response.memory.write((void*)(s.c_str()), s.length());

			Memory mem;
			mem.setSize(sz);
			f->read(mem.data, sz);
			response.memory.write(mem.data, sz);
		}
		delete f;
	}
}

void WebServerHandler::step(HttpRequest &request, HttpResponse &response) {
	int count = request.header.getCount();

	string s = "";
	for (int i = 0; i < count; i++) {
		string name = request.header.getName_s(i);
		string value = request.header.getValue_s(i);
		s = s + name + " = " + value + "\r\n";
	}

	count = request.header.GET.getCount();
	s = s + to_string(count) + "\r\n";

	for (int i = 0; i < count; i++) {
		String name = request.header.GET.getName(i);
		String value = request.header.GET.getValue(i);
		s = s + name.to_string() + " = " + value.to_string() + "\r\n";
	}

	s = "HTTP/1.1 200 OK\r\nContent-Length: " + to_string(s.length()) + "\r\n\r\n" + s + "\r\n";
	response.memory.write((void*)(s.c_str()), s.length());
}


//--------------------------------------------------------------------------------------------------
//----------          class WebServer          -----------------------------------------------------
//--------------------------------------------------------------------------------------------------

WebServer::WebServer(int port) {
	ss = new cj::ServerSocket();
	socketPort = port;
}

mutex g_mutex1;

void WebServer::threadFunction(Socket *socket)
{
	g_mutex1.lock();
	WebServerHandler *handler = new WebServerHandler();
	cout << "new " << handler << endl;
	g_mutex1.unlock();

	handler->threadStep(socket);

	g_mutex1.lock();
	cout << "del " << handler << endl;
	delete handler;
	g_mutex1.unlock();
}

void WebServer::run() {
	try {
		isRunning = true;
		bool flag = ss->create(AF_INET, SOCK_STREAM, 0);
		if (!flag)
		{
			exit(1);
		}
		if (!ss->bind(socketPort))
		{
			exit(2);
		}

		ss->setNonBlocking(true);
		ss->listen();

		int mycnt = 0;
		while (isRunning)
		{
			ss->accept();

			mycnt++;

			int index = 0;
			int count = ss->lstSocket.getCount();

			if (mycnt % 1000 == 0) {
				String s = "Accept. Count = " + (String)count;
				LOGGER_DEBUG(s);
			}

			for (int i = 0; i < count; i++) {
				Socket *socket = (Socket*)ss->lstSocket.getItem(index);
				index++;

				int size = socket->getCurSize();
				//				LOGGER_OUT("SIZE", "Size of Socket buffer = " + (String)size);

				if (size > 0) {
					index--;
					ss->lstSocket.del(index);

					LOGGER_DEBUG("----- i = " + (String)i + " size = " + (String)size);

					//std::thread *thr = new std::thread(&WebServer::threadStep, this, socket);
					std::thread *thr = new std::thread(&WebServer::threadFunction, this, socket);
					thr->detach();
					//lstThread.push_back(thr);
				}
			}
			usleep(1000);
		}
	}
	catch (...) {
		printf("Error in run() try catch(...)\n");
	}
}


void WebServer::setHandler(WebServerHandler *handler) {
	this->handler = handler;
}

void WebServer::threadStep(Socket *socket) {
	if (handler) handler->threadStep(socket);
}

}
