#include "JsonNode.h"


JsonValue::JsonValue() : JsonValue('?') {}
JsonValue::JsonValue(char c) : type(c), next(nullptr), member(nullptr), root(nullptr) {}

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
JsonValue::~JsonValue() {
    //puts("wiped");
}
std::ostream &JsonValue::print(std::ostream &os) const {
    return os << "[JsonValue]";
}
std::ostream &JsonValue::printJson(std::ostream &os) const {
    throw std::runtime_error("printJson this node should be overriden");
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

JsonPair::JsonPair(JsonState * state, char *s) : JsonValue('p') {
    key = state->getString(s);
}
JsonPair::~JsonPair() { }
std::ostream &JsonPair::print(std::ostream &os) const {
    os << "\"" << *key << "\"=>";
    member->print(os);
    return os;
}
std::ostream &JsonPair::printJson(std::ostream &os) const {
    os << "\"" << *key << "\":";
    member->printJson(os);
    return os;
}
int JsonPair::toLuaObject(LS) {
    member->toLuaObject(L);
    lua_setfield(L, -2, key->c_str());
    return 1;
}



///////////

JsonObject::JsonObject() : JsonValue('o') {}
std::ostream &JsonObject::print(std::ostream &os) const {
    os << "{JsonObject:";
    for (JsonValue *ele = member; ele; ele = ele->next) {
        os << " ";
        ele->print(os);
    }
    os << "}";
    return os;
}
std::ostream &JsonObject::printJson(std::ostream &os) const {
    os << "{";
    for (JsonValue *ele = member; ele; ele = ele->next) {
        ele->printJson(os);
        if (ele->next)
            os << ",";
    }
    os << "}";
    return os;
}
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

std::ostream &JsonArray::print(std::ostream &os) const {
    os << "[JsonArray:";
    for (JsonValue *ele = member; ele; ele = ele->next) {
        os << " ";
        ele->print(os);
    }
    os << "]";
    return os;
}
std::ostream &JsonArray::printJson(std::ostream &os) const {
    os << "[";
    for (JsonValue *ele = member; ele; ele = ele->next) {
        ele->printJson(os);
        if (ele->next)
            os << ",";
    }
    os << "]";
    return os;
}
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

JsonString::JsonString(JsonState * state, char * s) : JsonValue('s') {
    value = state->getString(s);
    s = nullptr;
}
JsonString::~JsonString() {}

std::ostream &JsonString::print(std::ostream &os) const {
    return os << "[JsonString: \"" << *value << "\"]";
}
std::ostream &JsonString::printJson(std::ostream &os) const {
    return os << "\"" << *value << "\"";
}
int JsonString::toLuaObject(LS) {
    lua_pushlstring(L, value->data(), value->length());
    return 1;
}

//////

JsonNumber::JsonNumber(double v) : JsonValue('n'), value(v) {}
std::ostream &JsonNumber::print(std::ostream &os) const {
    return os << "[JsonNumber: " << value << "]";
}
std::ostream &JsonNumber::printJson(std::ostream &os) const {
    return os << value;
}
int JsonNumber::toLuaObject(LS) {
    lua_pushnumber(L, value);
    return 1;
}

///

JsonBoolean::JsonBoolean(bool b) : JsonValue('b'), value(b) {}
std::ostream &JsonBoolean::print(std::ostream &os) const {
    return os << "[JsonBoolean: " << (value ? "true" : "false") << "]";
}
std::ostream &JsonBoolean::printJson(std::ostream &os) const {
    return os << (value ? "true" : "false");
}
int JsonBoolean::toLuaObject(LS) {
    lua_pushboolean(L, value);
    return 1;
}

//////// JsonNull

JsonNull::JsonNull() : JsonValue('x') {}

std::ostream &JsonNull::print(std::ostream &os) const {
    return os << "[JsonNull]";
}
std::ostream &JsonNull::printJson(std::ostream &os) const {
    return os << "null";
}
int JsonNull::toLuaObject(LS) {
    lua_pushnil(L);
    return 1;
}

JsonState::JsonState() : value(nullptr) {}
JsonState::~JsonState() {
    objList.clear();
    printf("freed, size: %lu\n", objList.size() );
}
const std::string * JsonState::getString( const char * str ){
    return &(*strPool.insert(str).first);
}

//JsonValue *JsonState::getJsonValue() const { return value.get(); }

void JsonState::free() {
}
