#ifndef _KEY_VALUE_CONTAINER
#define _KEY_VALUE_CONTAINER

#include <string>
#include <unordered_map>

template <typename T>
class key_value_container{
public:
	key_value_container(){
	}
	virtual ~key_value_container(){
	}

	void set(const std::string &key,const T &value){
		container[key] = value;
	}
	const T &get(const std::string &key){
		auto pair = container.find(key);

		if(pair == container.end())
			return novalue;

		return pair->second;
	}

	typename std::unordered_map<std::string,T>::iterator begin(){
		return container.begin();
	}
	typename std::unordered_map<std::string,T>::iterator end(){
		return container.end();
	}
	typename const std::unordered_map<std::string,T>::iterator cbegin(){
		return container.cbegin();
	}
	typename const std::unordered_map<std::string,T>::iterator cend(){
		return container.cend();
	}

	static T novalue;

protected:
	std::unordered_map<std::string,T> container;
};

template <typename T>
T key_value_container<T>::novalue;

#endif //_KEY_VALUE_CONTAINER