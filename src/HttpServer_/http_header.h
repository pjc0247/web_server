#ifndef _HTTP_HEADER_H
#define _HTTP_HEADER_H

#include <string>

#include "string_container.h"

namespace http{

class header{
public:
	header();
	virtual ~header();

	const std::string &get_field(const std::string &key);
	header *set_field(const std::string &key, const std::string &value);

	bool load(const std::string &header);
	std::string compile();

	const std::string &get_method();
	const std::string &get_request_uri();
	const std::string &get_version();

	void set_method(const std::string &method);
	void set_request_uri(const std::string &request_uri);
	void set_version(const std::string &version);

private:
	std::string method;
	std::string request_uri;
	std::string version;

	string_container fields;
};

};

#endif //_HTTP_HEADER_H