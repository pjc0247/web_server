#include "stdafx.h"
#include "http_header.h"

#include <regex>

using namespace std;

namespace http{

header::header(){
}
header::~header(){
}

const string &header::get_field(const string &key){
	return fields.get(key);
}
header *header::set_field(const string &key, const string &value){
	fields.set(key, value);
	return this;
}

bool header::load(const string &header){
	regex re_pre_header("^(GET|POST) (.+) (HTTP/1\\.1|HTTP/1\\.0)");
	regex rg_field("([a-zA-Z_-]*)\\s?:\\s?(.*)");
	smatch match;

	string tmp = header;

	regex_search(tmp, match, re_pre_header);
	if(match.size() == 4){
		method = match[1].str();
		request_uri = match[2].str();
		version = match[3].str();
	}

	while( regex_search(tmp, match,rg_field) ){
		string key = match[1].str();
		string value = match[2].str();

		transform( key.begin(),key.end(),key.begin(), ::tolower );
		
		fields.set( key, value );

		tmp = match.suffix().str();
	}

	return true;
}
string header::compile(){
	string header;

	for(auto &pair : fields){
		header += pair.first + ":" + pair.second;
		header += "\r\n";
	}
	header += "\r\n";

	return header;
}

const string &header::get_method(){
	return method;
}
const string &header::get_request_uri(){
	return request_uri;
}
const string &header::get_version(){
	return version;
}

void header::set_method(const string &_method){
	method = _method;
}
void header::set_request_uri(const string &_request_uri){
	request_uri = _request_uri;
}
void header::set_version(const string &_version){
	version = _version;
};

};