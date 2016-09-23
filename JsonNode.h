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
#include <exception>
#include <stdexcept>
#include <deque>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <vector>

#include <cassert>

struct JsonState;

union JsonBaseType {
	double n;
	bool b;
	char *s;
};

struct JsonValue {
    char type;
    JsonValue *member;
    JsonState *root;

	bool isRoot = false;

    JsonValue();
    JsonValue(char);

    ~JsonValue();

    virtual int toLuaObject(LS);

    int asLuaObject(LS);
    int luaGet(LS);
    int luaLen(LS);

    bool isBaseType() const;

    const char * typeString() const;

    template<class T> T* as(){
        return dynamic_cast<T*>(this);
    }

};

struct JsonString;

struct JsonObject : public JsonValue {

    JsonObject();

    std::unordered_map<const char* , JsonValue*> ptrTable;

    virtual int toLuaObject(LS);
};

struct JsonArray : public JsonValue {

    std::vector<JsonValue*> ptrVec;

    JsonArray();

    virtual int toLuaObject(LS);

};

struct JsonString : public JsonValue {
    const char* value;
    JsonString(JsonState *, const char *);
    ~JsonString();

    virtual int toLuaObject(LS);
};
struct JsonNumber : public JsonValue {
    double value;
    JsonNumber(double v);
    virtual int toLuaObject(LS);
};
struct JsonBoolean : public JsonValue {
    bool value;
    JsonBoolean(bool b);
    virtual int toLuaObject(LS);
};
struct JsonNull : public JsonValue {

    JsonNull();

    virtual int toLuaObject(LS);
};

struct JsonState {
	JsonValue *value;
    std::deque< std::unique_ptr<JsonValue> > objList;

    std::unordered_set< std::string > strPool;

    JsonState();
    ~JsonState();

    const char * getString( const char * );
    const char * getString( std::string & );

    template <class T, class... Args> T *newObject(Args && ... args) {
        static_assert(std::is_base_of<JsonValue, T>::value,
                      "must derived from JsonValue");

        auto p = std::make_unique<T>( std::forward<Args>(args)... ) ;
		p->root = this;
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

