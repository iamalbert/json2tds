#ifndef JSON_NODE_H
#define JSON_NODE_H

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#define LS lua_State *L

#define LUA_INDEX_BASE (1)

#include <vector>
#include <string>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <deque>
#include <memory>
#include <map>
#include <set>
#include <vector>

#include <cassert>

struct JsonNode;

struct JsonNode {
    union {
        int i;
        double d;
        char * s;
        std::map<std::string, JsonNode> o;
        std::vector<JsonNode> a;
    } value;

    char type; // i(nt),d(ouble),s(tring),o(bject),a(rray),n(ull)
};


struct JsonState;

struct JsonValue {
    char type;
    JsonValue *next, *member;
    JsonState *root;

    JsonValue();
    JsonValue(char);



    virtual ~JsonValue();

    virtual void breakLinks();
    virtual std::ostream &print(std::ostream &os) const;
    virtual std::ostream &printJson(std::ostream &os) const;

    virtual int toLuaObject(LS);
    int asLuaObject(LS);

    int luaGet(LS, JsonState * = 0);
    int luaLen(LS);
    bool isBaseType() const;
    const char * typeString() const;

    template<class T> T* as(){
        return dynamic_cast<T*>(this);
    }

    JsonValue *reverse_member();

};

struct JsonString;

struct JsonPair : public JsonValue {
	const std::string* key;
    JsonPair(JsonState *, char *);

    virtual ~JsonPair();
    virtual std::ostream &print(std::ostream &os) const;
    virtual std::ostream &printJson(std::ostream &os) const;

    virtual int toLuaObject(LS);
};

struct JsonObject : public JsonValue {

    JsonObject();

    std::map<const std::string*, JsonValue*> ptrTable;

    virtual std::ostream &print(std::ostream &os) const;
    virtual std::ostream &printJson(std::ostream &os) const;
    virtual int toLuaObject(LS);
    void list2map();
};

struct JsonArray : public JsonValue {

    std::vector<JsonValue*> ptrVec;

    JsonArray();

    virtual std::ostream &print(std::ostream &os) const;
    virtual std::ostream &printJson(std::ostream &os) const;
    virtual int toLuaObject(LS);

    void list2vector();
};

struct JsonString : public JsonValue {
    const std::string* value;
    JsonString(JsonState *, char *);
    ~JsonString();

    virtual std::ostream &print(std::ostream &os) const;
    virtual std::ostream &printJson(std::ostream &os) const;
    virtual int toLuaObject(LS);
};
struct JsonNumber : public JsonValue {
    double value;
    JsonNumber(double v);
    virtual std::ostream &print(std::ostream &os) const;
    virtual std::ostream &printJson(std::ostream &os) const;
    virtual int toLuaObject(LS);
};
struct JsonBoolean : public JsonValue {
    bool value;
    JsonBoolean(bool b);
    virtual std::ostream &print(std::ostream &os) const;
    virtual std::ostream &printJson(std::ostream &os) const;
    virtual int toLuaObject(LS);
};
struct JsonNull : public JsonValue {

    JsonNull();

    virtual std::ostream &print(std::ostream &os) const;
    virtual std::ostream &printJson(std::ostream &os) const;
    virtual int toLuaObject(LS);
};

struct JsonState {
	JsonValue *value;
    std::deque< std::unique_ptr<JsonValue> > objList;

    std::set< std::string > strPool;

    JsonState();
    ~JsonState();

    const std::string * getString( const char * );

    template <class T, class... Args> T *newObject(Args && ... args) {
        static_assert(std::is_base_of<JsonValue, T>::value,
                      "must derived from JsonValue");

        auto p = std::make_unique<T>( std::forward<Args>(args)... ) ;
        //auto p = std::unique_ptr<T>( ) ;
        objList.push_back( std::move(p) );

        return static_cast<T*>(objList.back().get());
    }

    //JsonValue *getJsonValue() const;

    void free();
};

struct JsonException : public std::runtime_error {
    template <class... Args>
    JsonException(Args &&... args)
        : std::runtime_error(std::forward<Args>(args)...) {}
};

JsonState* parse_json(FILE *);
JsonState* parse_json_string(const char *);

#endif
