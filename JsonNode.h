#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include <deque>

struct JsonValue {
	JsonValue * next;
	JsonValue * member;
	JsonValue():
		next( nullptr ) , member( nullptr ) 
	{}

	virtual void breakLinks(){
		next = nullptr;
		member = nullptr;
	}

    virtual ~JsonValue() {
		if( next ){
			delete next;
			next = nullptr;
		}
		if( member ){
			delete member;
			member = nullptr;
		}
	}
	virtual std::ostream & print( std::ostream & os ) const {
		return os << "[JsonValue]";
	}
};

struct JsonString;

struct JsonPair : public JsonValue {
	std::string* key;
	JsonPair(std::string* k):
		key(k) {}

	~JsonPair(){
		if( key ) {
			delete key;
			key = nullptr;
		}
	}

	virtual std::ostream & print( std::ostream & os ) const {
		os << "\"" << *key << "\"=>";
		member->print(os);
		return os;
	}
};


struct JsonObject : public JsonValue {
	virtual std::ostream & print( std::ostream & os ) const {
		os << "{JsonObject:";
		for( JsonValue * ele = member; ele; ele = ele->next ){
			os << " " ;
			ele->print(os);
		}
		os << "}";
		return os;
	}

};

struct JsonArray : public JsonValue {
	virtual std::ostream & print( std::ostream & os ) const {
		os << "[JsonArray:";
		for( JsonValue * ele = member; ele; ele = ele->next ){
			os << " " ;
			ele->print(os);
		}
		os << "]";
		return os;
	}

};

struct JsonString : public JsonValue {
    std::string *value;
	JsonString( std::string *s) : value(s) {
	}
	~JsonString(){
		if(value){
			delete value;
			value = nullptr;
		}
	}

	virtual std::ostream & print( std::ostream & os ) const {
		return os << "[JsonString: \"" << *value << "\"]";
	}
};
struct JsonNumber : public JsonValue {
    JsonNumber(double v) : value(v) {}
    double value;
	virtual std::ostream & print( std::ostream & os ) const {
		return os << "[JsonNumber: " << value << "]";
	}
};
struct JsonBoolean : public JsonValue {
	JsonBoolean(bool b): value(b) {}
    bool value;
	virtual std::ostream & print( std::ostream & os ) const {
		return os << "[JsonBoolean: " << (value?"true":"false") << "]";
	}
};
struct JsonNull : public JsonValue {
	virtual std::ostream & print( std::ostream & os ) const {
		return os << "[JsonNull]";
	}
};


struct JsonState {
	JsonValue * value;

	std::deque<JsonValue*> objList;
	JsonState() : value(nullptr){
	}
	~JsonState(){
	}

	template<class T, class ...Args>
	T* newObject( Args&& ... args ){
		static_assert( std::is_base_of< JsonValue, T >::value, 
				"must derived from JsonValue");
		T* obj = new T( std::forward<Args>(args)... );
		objList.push_back(obj);
		return obj;
	}

	JsonValue * getJsonValue() const {
		return value;
	}

	void free(){
		for( auto ptr : objList ){
			ptr->breakLinks();
			delete ptr;
		}
		objList.clear();
		value = nullptr;
	}
};


struct JsonException : public std::runtime_error {
	JsonException( const std::string & error ) :
		std::runtime_error( error ){}
	JsonException( const char * error ) :
		std::runtime_error( error ){}
};
