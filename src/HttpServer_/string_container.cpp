#include "stdafx.h"

#include "string_container.h"

using namespace std;

string_container::string_container(){
}
string_container::~string_container(){
}

bool string_container::load(const std::string &str){
	int offset = 0;
	int length = str.length();

	string key, value;

	for(int i=0;i<length;i++){
		if( str[i] == '=' ){
			key = str.substr( offset, i-offset );
			offset = i+1;
		}
		else if( str[i] == ';' ){
			value = str.substr( offset, i-offset );
			offset = i+1;

			set( key, value );
		}
		// ignore whitespace
		else if( str[i] == ' ' ){
			offset ++;
		}
	}

	if( offset != length ){
		value = str.substr( offset, length-offset );

		set( key, value );
	}

	return true;
}