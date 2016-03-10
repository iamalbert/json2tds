
#include "JsonNode.h"

JsonValue::JsonValue():
	next( nullptr ) , member( nullptr ) 
{}

void JsonValue::breakLinks(){
	next = nullptr;
	member = nullptr;
}

JsonValue::~JsonValue() {
	if( next ){
		delete next;
		next = nullptr;
	}
	if( member ){
		delete member;
		member = nullptr;
	}
}
std::ostream & JsonValue::print( std::ostream & os ) const {
	return os << "[JsonValue]";
}



JsonPair::JsonPair(std::string* k):
	key(k) {}

JsonPair::~JsonPair(){
	if( key ) {
		delete key;
		key = nullptr;
	}
}

std::ostream & JsonPair::print( std::ostream & os ) const {
	os << "\"" << *key << "\"=>";
	member->print(os);
	return os;
}





std::ostream & JsonObject::print( std::ostream & os ) const {
	os << "{JsonObject:";
	for( JsonValue * ele = member; ele; ele = ele->next ){
		os << " " ;
		ele->print(os);
	}
	os << "}";
	return os;
}






std::ostream & JsonArray:: print( std::ostream & os ) const {
	os << "[JsonArray:";
	for( JsonValue * ele = member; ele; ele = ele->next ){
		os << " " ;
		ele->print(os);
	}
	os << "]";
	return os;
}





JsonString:: JsonString( std::string *s) : value(s) {
}
JsonString::~JsonString(){
	if(value){
		delete value;
		value = nullptr;
	}
}

std::ostream & JsonString::print( std::ostream & os ) const {
	return os << "[JsonString: \"" << *value << "\"]";
}






JsonNumber::JsonNumber(double v) : value(v) {}
std::ostream & JsonNumber::print( std::ostream & os ) const {
	return os << "[JsonNumber: " << value << "]";
}





JsonBoolean::JsonBoolean(bool b): value(b) {}
std::ostream & JsonBoolean:: print( std::ostream & os ) const {
	return os << "[JsonBoolean: " << (value?"true":"false") << "]";
}


std::ostream & JsonNull::print( std::ostream & os ) const {
	return os << "[JsonNull]";
}


JsonState::JsonState() : value(nullptr){ }

JsonValue * JsonState::getJsonValue() const { return value; }

void JsonState::free(){
	for( auto ptr : objList ){
		ptr->breakLinks();
		delete ptr;
	}
	objList.clear();
	value = nullptr;
}


