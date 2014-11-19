#ifndef _STRING_CONTAINER_H
#define _STRING_CONTAINER_H

#include <string>

#include "key_value_container.h"

class string_container : public key_value_container<std::string>{
public:
	string_container();
	virtual ~string_container();
	
	bool load(const std::string &str);
};

#endif //_STRING_CONTAINER_H