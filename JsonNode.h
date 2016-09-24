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
    JsonState *root;

	bool isRoot = false;

    JsonValue();
    JsonValue(char);

    virtual ~JsonValue() = default;

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

    std::unordered_map<const char* , JsonValue*> ptrTable;

    JsonObject();
    virtual ~JsonObject() = default;
    virtual int toLuaObject(LS);
};

struct JsonArray : public JsonValue {

    std::vector<JsonValue*> ptrVec;

    JsonArray();
    virtual ~JsonArray() = default;
    virtual int toLuaObject(LS);

};

struct JsonString : public JsonValue {
    const char* value;
    JsonString(const char *);
    virtual ~JsonString() = default;
    virtual int toLuaObject(LS);
};
struct JsonNumber : public JsonValue {
    double value;
    JsonNumber(double v);
    virtual ~JsonNumber() = default;
    virtual int toLuaObject(LS);
};
struct JsonBoolean : public JsonValue {
    bool value;
    JsonBoolean(bool b);
    virtual ~JsonBoolean() = default;
    virtual int toLuaObject(LS);
};
struct JsonNull : public JsonValue {
    JsonNull();
    virtual ~JsonNull() = default ;
    virtual int toLuaObject(LS);
};

struct JsonState {
	JsonValue *value;

    std::deque< std::unique_ptr<JsonValue> > objList;
    std::unordered_set< std::string > strPool;

    JsonState();
    ~JsonState() = default;

    const char * getString( const char * );
    const char * getString( std::string & );

    template <class T, class... Args> T *newObject(Args && ... args) {
        static_assert(std::is_base_of<JsonValue, T>::value,
                      "must derived from JsonValue");

        //auto p = std::make_unique<T>( std::forward<Args>(args)... ) ;
		//p->root = this;
        //auto p = std::unique_ptr<T>( ) ;
        //objList.push_back( std::move(p) );

        objList.emplace_back( new T(std::forward<Args>(args)...) );
        objList.back()->root = this;

        return static_cast<T*>(objList.back().get());
    }

    //JsonValue *getJsonValue() const;

    void free();
};

JsonState* parse_json(FILE *);
JsonState* parse_json_string(const char *);

#endif

