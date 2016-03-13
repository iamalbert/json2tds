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
std::ostream & JsonValue::printJson( std::ostream & os ) const {
    throw std::runtime_error("printJson this node should be overriden");
}
int JsonValue::toLuaObject(LS){
    throw std::runtime_error("toLuaObject this node should be overriden");
}
    int asLuaObject(LS);

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
std::ostream & JsonPair::printJson( std::ostream & os ) const {
	os << "\"" << *key << "\":";
	member->printJson(os);
    return os;
}
int JsonPair::toLuaObject(LS){
    member->toLuaObject(L);
    lua_setfield(L, -2, key->c_str() );
    return 1;
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
std::ostream & JsonObject::printJson( std::ostream & os ) const {
	os << "{";
	for( JsonValue * ele = member; ele; ele = ele->next ){
		ele->printJson(os);
        if( ele->next ) os << ",";
	}
	os << "}";
	return os;
}
int JsonObject::toLuaObject(LS){
    lua_newtable(L);
	for( JsonPair* ele = (JsonPair*)member; ele; 
            ele = (JsonPair*)ele->next )
    {
		ele->member->toLuaObject(L);
        lua_setfield(L, -2, ele->key->c_str() );
    }
    return 1;
}








std::ostream & JsonArray:: print( std::ostream & os ) const {
	os << "[JsonArray:";
	for( JsonValue * ele = member; ele; ele = ele->next ){
		os << " " ;
		ele->print(os);
	}
	os << "]";
	return os;
}std::ostream & JsonArray::printJson( std::ostream & os ) const {
	os << "[";
	for( JsonValue * ele = member; ele; ele = ele->next ){
		ele->printJson(os);
        if( ele->next ) os << ",";
	}
	os << "]";
	return os;
}
int JsonArray::toLuaObject(LS){
    lua_newtable(L);
    int idx = 1;
	for( JsonValue *ele = member; ele; ele = ele->next ){
        ele->toLuaObject(L);
        lua_rawseti(L, -2, idx );
    }
    return 1;
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
std::ostream & JsonString::printJson( std::ostream & os ) const {
	return os << "\"" << *value << "\"";
}
int JsonString::toLuaObject(LS)
{
    lua_pushlstring(L, value->data(), value->length() );
    return 1;
}



JsonNumber::JsonNumber(double v) : value(v) {}
std::ostream & JsonNumber::print( std::ostream & os ) const {
	return os << "[JsonNumber: " << value << "]";
}
std::ostream & JsonNumber::printJson( std::ostream & os ) const {
	return os << value;
}
int JsonNumber::toLuaObject(LS)
{
    lua_pushnumber(L, value);
    return 1;
}





JsonBoolean::JsonBoolean(bool b): value(b) {}
std::ostream & JsonBoolean:: print( std::ostream & os ) const {
	return os << "[JsonBoolean: " << (value?"true":"false") << "]";
}
std::ostream & JsonBoolean:: printJson( std::ostream & os ) const {
	return os << (value?"true":"false");
}
int JsonBoolean::toLuaObject(LS){
    lua_pushboolean(L, value);
    return 1;
}


std::ostream & JsonNull::print( std::ostream & os ) const {
	return os << "[JsonNull]";
}
std::ostream & JsonNull::printJson( std::ostream & os ) const {
	return os << "null";
}
int JsonNull::toLuaObject(LS){
    lua_pushnil(L);
    return 1;
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
