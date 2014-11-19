#include "server.h"

#include "http.h"
#include "http_header.h"
#include "field_name.h"

#define _STRCMP4(str, a1,a2,a3,a4) \
    (*(unsigned int*)(str) == ((a4<<24) | (a3<<16) | (a2<<8) | a1))

using namespace std;

namespace http{

server::server() :
	workers(8, 32, 50,
		std::bind(&server::on_connected, this, placeholders::_1)){
}
server::~server(){
}

bool server::open(
	const string &host, int port){

#ifdef TARGET_WIN32
	WSAData wsa;
	WSAStartup(MAKEWORD(2,2), &wsa);
#endif

	sock = socket(PF_INET, SOCK_STREAM, 0);   
	if(sock == 0)
		return false;

	hostent* host_info =
		gethostbyname(host.c_str());
	if(host_info == nullptr)
		return false;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=
		*(unsigned long*)host_info->h_addr_list[0];
	addr.sin_port=htons(port);

	int optval = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));

	/* bind */
	if(::bind(sock, (sockaddr*)&addr, sizeof(addr)) == -1)
		return false;

	/* listen */
	if(listen(sock, SOMAXCONN) == -1)
		return false;

	return true;
}
bool server::run(){

	while(true){
		unsigned int client;
		sockaddr_in addr;
		int addr_size = sizeof(addr);

		client = ::accept(sock, (sockaddr*)&addr, &addr_size);
		if(client > 0){
			workers.enqueue(client);
		}
	}

	return true;
}

bool server::on_connected(unsigned int client){
	string header;
	string document;
	bool received = false;

	while(!received){
		char buffer[buffer_size];
		int len;

		len = recv(client, buffer, buffer_size, 0);
		if(len < 0) return true;

		buffer[len] = '\0';
		header.append(buffer);

		printf("%s", buffer);

		if(header.length() >= 3){
			const char *cstr = header.c_str();
			int start =	max((header.size() - len -3), 0);
			int end = header.size();

			for(int i=start;i<end;i++){
				if(_STRCMP4(cstr+i, '\r', '\n', '\r', '\n')){

					document = header.substr(i+4);
					header = header.erase(i+4);
					received = true;
				}
			}
		}
	}

	http::header http_header;
	http_header.load(header);

	send_response(
		client, http_header.get_request_uri());

	return true;
}
void server::send_response(
	unsigned int client,
	const string &request_uri){

	http::header http_header;
	char length[128];
	int code = 200;
	string os_path = request_uri.substr(1);
	size_t size = 0;

	replace(
		os_path.begin(), os_path.end(),
		'/', '\\');
	FILE *fp = fopen(
		os_path.c_str(), "rb");

	if(fp == nullptr){
		code = 404;
		fp = fopen("404.html", "rb");
	}

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	

	/* HttpVer / StatusCode / ReasonPhrase */
	char pre_header[128];
	sprintf(pre_header,
		"%s %d %s\r\n",
		http::version_11.c_str(),
		code,
		"OK");

	sprintf(length, "%d", size);

	http_header
		.set_field(http::fields::server_name, "hello")
		->set_field(http::fields::connection, "close")
		->set_field(http::fields::content_length, length);


	auto header = http_header.compile();

	printf("%s", pre_header);
	printf("%s", header.c_str());
	send(client, pre_header, strlen(pre_header), 0);
	send(client, header.c_str(), header.size(), 0);

	if(fp){
		while(true){
			char buffer[buffer_size];
			size_t len;

			len = fread(buffer, 1, buffer_size, fp);
			send(client, buffer, len, 0);

			if(len != buffer_size) break;
		}
	}

	closesocket(client);
}

};