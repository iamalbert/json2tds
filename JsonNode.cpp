#include "JsonNode.h"


JsonValue::JsonValue() : JsonValue('?') {}
JsonValue::JsonValue(char c) : type(c){
	//printf("value %c\n", c);
}
int JsonValue::toLuaObject(LS) {
    switch(type){
        case 'n':
            lua_pushnumber(L, as<JsonNumber>()->value);
            return 1;
        case 'b':
            lua_pushboolean(L, as<JsonBoolean>()->value);
            return 1;
        case 's': 
            lua_pushstring(L, as<JsonString>()->value);
            return 1;
        case 'x':
            lua_pushnil(L);
            return 1;
        case 'o': 
            lua_newtable(L);
            for ( auto & kv : as<JsonObject>()->ptrTable ){
                kv.second->toLuaObject(L);
                lua_setfield(L, -2, kv.first );
            }
            return 1;
        case 'a':{
            lua_newtable(L);
            int idx = 1;
            for ( auto & v : as<JsonArray>()->ptrVec ){
                v->toLuaObject(L);
                lua_rawseti(L, -2, idx);
                idx++;
            }
            return 1;
        }
        default:
            luaL_error(L, "unknown type of JsonValue: %c", type);
            return 0;
    }
}
const char * JsonValue::typeString() const {
    switch(type){
        case 'n': return "JsonNumber";
        case 'b': return "JsonBoolean";
        case 's': return "JsonString";
        case 'o': return "JsonObject";
        case 'a': return "JsonArray";
        case 'x': return "JsonNull";
        default:
                  return "JsonUnknownType";
    }
}



///////////

JsonObject::JsonObject() : JsonValue('o') {}

/////////

JsonArray::JsonArray() : JsonValue('a') {}


/////

JsonString::JsonString(const char * s) : JsonValue('s'), value(s) {
}


//////

JsonNumber::JsonNumber(double v) : JsonValue('n'), value(v) {}

///

JsonBoolean::JsonBoolean(bool b) : JsonValue('b'), value(b) {}

//////// JsonNull

JsonNull::JsonNull() : JsonValue('x') {}

JsonState::JsonState() : value(nullptr), refcnt(1) {
}
const char * JsonState::getString( const char * str ){
	auto s = strPool.insert(str);
    return s.first->data();
}
const char * JsonState::searchString( const char * str ){
    auto it = strPool.find(str);
    if( it == strPool.end() ){ 
        return nullptr;
    }else{
        return it->data();
    }
}

//JsonValue *JsonState::getJsonValue() const { return value.get(); }


void JsonState::free() {
    for( auto & v : objList ){
        delete v;
        v = nullptr;
    }
    value = nullptr;
    objList.clear();
    objList.shrink_to_fit();
}
