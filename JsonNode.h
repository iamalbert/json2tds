#include <vector>
#include <string>
#include <iostream>

struct JsonValue {
	JsonValue * next;
	JsonValue * member;
	JsonValue():
		next(   (JsonValue*) 0 ) ,
		member( (JsonValue*) 0 ) 
	{}

    virtual ~JsonValue() {
		if( next ){
			delete next;
		}
		if( member ){
			delete member;
		}
	}
	virtual std::ostream & print( std::ostream & os ) const {
		return os << "[JsonValue]";
	}
};

struct JsonString;

struct JsonPair : public JsonValue {
	std::string* key;
    JsonValue*  value;
	JsonPair(std::string* k, JsonValue * v):
		key(k), value(v) {}

	~JsonPair(){
		if( key ) delete key;
		if( value ) delete value;
	}

	virtual std::ostream & print( std::ostream & os ) const {
		os << "\"" << *key << "\"=>";
		value->print(os);
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
		if(value) delete value;
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
	JsonState(){ }
};

