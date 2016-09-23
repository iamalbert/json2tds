#include "JsonNode.h"


JsonValue::JsonValue() : JsonValue('?') {}
JsonValue::JsonValue(char c) : type(c), 
    member(nullptr), 
    root(nullptr) 
{
	//printf("value %c\n", c);
}
JsonValue::~JsonValue() {
   //printf("wiped %c\n", type);
}

const char * JsonValue::typeString() const {
    switch(type){
        case 'n': return "JsonNumber";
        case 'b': return "JsonBoolean";
        case 's': return "JsonString";
        case 'o': return "JsonObject";
        case 'a': return "JsonArray";
        default:
                  return "JsonUnknownType";
    }
}
int JsonValue::toLuaObject(LS) {
    throw std::runtime_error("toLuaObject this node should be overriden");
}

int JsonValue::asLuaObject(LS) {
    int old_stack_size = lua_gettop(L);
    this->toLuaObject(L);
    int new_stack_size = lua_gettop(L);
    assert(old_stack_size + 1 == new_stack_size);
    return 1;
}


///////////

JsonObject::JsonObject() : JsonValue('o') {}
int JsonObject::toLuaObject(LS) {
    lua_newtable(L);
    for ( auto & kv : ptrTable ){
        kv.second->toLuaObject(L);
        lua_setfield(L, -2, kv.first );
    }
    return 1;
}

/////////

JsonArray::JsonArray() : JsonValue('a') {}

int JsonArray::toLuaObject(LS) {
    lua_newtable(L);
    int idx = 1;
    for ( auto & v : ptrVec ){
        v->toLuaObject(L);
        lua_rawseti(L, -2, idx);
        idx++;
    }
    return 1;
}

/////

JsonString::JsonString(const char * s) : JsonValue('s'), value(s) {
}
JsonString::~JsonString() {}

int JsonString::toLuaObject(LS) {
    lua_pushstring(L, value );
    return 1;
}

//////

JsonNumber::JsonNumber(double v) : JsonValue('n'), value(v) {}
int JsonNumber::toLuaObject(LS) {
    lua_pushnumber(L, value);
    return 1;
}

///

JsonBoolean::JsonBoolean(bool b) : JsonValue('b'), value(b) {}
int JsonBoolean::toLuaObject(LS) {
    lua_pushboolean(L, value);
    return 1;
}

//////// JsonNull

JsonNull::JsonNull() : JsonValue('x') {}
int JsonNull::toLuaObject(LS) {
    lua_pushnil(L);
    return 1;
}

JsonState::JsonState() : value(nullptr) {
	//puts("value state");
}
JsonState::~JsonState() {
    //printf("wiped state, size: %lu\n", objList.size() );
    objList.clear();
}
const char * JsonState::getString( const char * str ){
	//for( auto & v : strPool ){ std::cout << "g:" << v << " " << &v << "\n"; }
	//std::cout << std::string(str) << "\n";
	auto s = strPool.insert(str);
    //printf("getstring: `%s', %p, %d\n", s.first->data(), (void*)s.first->data(), s.second);
    return s.first->data();
}
const char * JsonState::getString( std::string & str ){
	return getString( str.data() );
	/*
	auto s = strPool.insert(str);
    return &( *s.first );
	*/
}

//JsonValue *JsonState::getJsonValue() const { return value.get(); }

void JsonState::free() {
}
