#include "JsonNode.h"


JsonValue::JsonValue() : JsonValue('?') {}
JsonValue::JsonValue(char c) : type(c), 
    next(nullptr), member(nullptr), 
    tail(nullptr),root(nullptr) 
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
void JsonValue::breakLinks() {
    next = nullptr;
    member = nullptr;
}
JsonValue* JsonValue::reverse_member(){
    JsonValue  
        *curr = member,
        *next = nullptr,
        *prev = nullptr;

    while( curr ){
        next = curr->next;
        curr->next = prev;

        prev = curr;
        curr = next;
    }
    member = prev;
    return this;
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

JsonPair::JsonPair(JsonState * state, std::string *s) : JsonValue('p') {
    key = state->getString(*s);
}
JsonPair::~JsonPair() { }
int JsonPair::toLuaObject(LS) {
    member->toLuaObject(L);
    lua_setfield(L, -2, key->c_str());
    return 1;
}



///////////

JsonObject::JsonObject() : JsonValue('o') {}
int JsonObject::toLuaObject(LS) {
    lua_newtable(L);
    for ( auto & kv : ptrTable ){
        kv.second->toLuaObject(L);
        lua_setfield(L, -2, kv.first->c_str() );
    }
    return 1;
}
void JsonObject::list2map(){
    for (JsonPair *ele = (JsonPair *)member; ele; ele = (JsonPair *)ele->next) {
        ptrTable[ ele->key ] = ele->member;
    }
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
void JsonArray::list2vector(){
    for (JsonValue *ele = member; ele; ele = ele->next) {
        ptrVec.push_back(ele);
    }
}

/////

JsonString::JsonString(JsonState * state, std::string * s) : JsonValue('s') {
    value = state->getString(*s);
}
JsonString::~JsonString() {}

int JsonString::toLuaObject(LS) {
    lua_pushlstring(L, value->data(), value->length());
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
const std::string * JsonState::getString( const char * str ){
	//for( auto & v : strPool ){ std::cout << "g:" << v << " " << &v << "\n"; }
	//std::cout << std::string(str) << "\n";
	auto s = strPool.insert(str);
	//std::cout << str << " " <<  s.second << "\n";
    return &( *s.first );
}
const std::string * JsonState::getString( std::string & str ){
	return getString( str.data() );
	/*
	auto s = strPool.insert(str);
    return &( *s.first );
	*/
}

//JsonValue *JsonState::getJsonValue() const { return value.get(); }

void JsonState::free() {
}
