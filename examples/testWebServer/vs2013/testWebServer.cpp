#include "cj.h"
#include "cjNetwork.h"
using namespace cj;

#include "myWebServer.h"

#ifdef OS_WINDOWS
#pragma comment(lib, "cjCore.lib")
#pragma comment(lib, "cjNetwork.lib")
#endif

int main()
{
	MyWebServer *ws = new MyWebServer(8080);
	application = ws;
	ws->runLight();

	return 0;
}

