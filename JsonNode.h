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

    JsonValue();
    JsonValue(char);

    virtual ~JsonValue() = default;

    int toLuaObject(LS);
    int luaGet(LS);
    int luaLen(LS);

    bool isBaseType() const;
    bool isRoot() const;

    const char * typeString() const;

    template<class T> T* as(){
        return static_cast<T*>(this);
    }

};

struct JsonString;

struct JsonObject : public JsonValue {

    std::unordered_map<const char* , JsonValue*> ptrTable;

    JsonObject();
    virtual ~JsonObject() = default;
};

struct JsonArray : public JsonValue {

    std::vector<JsonValue*> ptrVec;

    JsonArray();
    virtual ~JsonArray() = default;

};

struct JsonString : public JsonValue {
    const char* value;
    JsonString(const char *);
    virtual ~JsonString() = default;
};
struct JsonNumber : public JsonValue {
    double value;
    JsonNumber(double v);
    virtual ~JsonNumber() = default;
};
struct JsonBoolean : public JsonValue {
    bool value;
    JsonBoolean(bool b);
    virtual ~JsonBoolean() = default;
};
struct JsonNull : public JsonValue {
    JsonNull();
    virtual ~JsonNull() = default ;
};

struct JsonState {
	JsonValue *value;

    std::vector< JsonValue* > objList;
    std::unordered_set< std::string > strPool;

    JsonState();
    ~JsonState() = default;

    const char * getString( const char * );

    template <class T, class... Args> T *newObject(Args && ... args) {
        static_assert(std::is_base_of<JsonValue, T>::value,
                      "must derived from JsonValue");

        //auto p = std::make_unique<T>( std::forward<Args>(args)... ) ;
		//p->root = this;
        //auto p = std::unique_ptr<T>( ) ;
        //objList.push_back( std::move(p) );
        T* obj = new T(args...);
        obj->root = this;

        objList.push_back(obj);

        return obj;
    }

    //JsonValue *getJsonValue() const;

    void free();
};

bool parse_json(FILE *, JsonState*);
bool parse_json_string(const char *, JsonState*);

#endif

