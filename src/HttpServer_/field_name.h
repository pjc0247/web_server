#ifndef _FIELD_NAME_H
#define _FIELD_NAME_H

#include <string>

#define _KEY(key, key_real) \
	const std::string key = key_real

namespace http{
namespace fields{
	_KEY(content_length, "content-length");
	_KEY(accept, "accept");

	_KEY(server_name, "server");
	_KEY(content_type, "content-type");
	_KEY(connection, "connection");
};};

#endif //_FIELD_NAME_H