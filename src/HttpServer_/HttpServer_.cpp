#include "stdafx.h"

#include <WinSock2.h>
#include <Windows.h>

#include "server.h"

#pragma comment (lib, "ws2_32")

using namespace std;

int main(int argc, char **argv){
	http::server *s = new http::server();

	s->open("0.0.0.0", 8081);
	s->run();

	return 0;
}