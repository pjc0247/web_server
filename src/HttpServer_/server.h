#ifndef _SERVER_H
#define _SERVER_H

#include "config.h"
#include "process_pool.h"

#include <string>

namespace http{
class server{
public:
	static const int buffer_size = 1024;

public:
	server();
	virtual ~server();

	bool open(
		const std::string &host, int port);
	bool run();

	bool on_connected(unsigned int clinet);

	void send_response(
		unsigned int client,
		const std::string &request_uri);

private:
	unsigned int sock;
	sockaddr_in addr;
	process_pool<unsigned int> workers;
};};

#endif //_SERVER_H