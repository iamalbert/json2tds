#ifndef JSON_NODE_H
#define JSON_NODE_H

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#define LS lua_State *L

#include <vector>
#include <string>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <deque>
#include <memory>

#include <cassert>

struct JsonValue {
	JsonValue *next, *member;
    JsonValue();

    virtual ~JsonValue();

    virtual void breakLinks();
    virtual std::ostream &print(std::ostream &os) const;
    virtual std::ostream &printJson(std::ostream &os) const;
    virtual int toLuaObject(LS);
    int asLuaObject(LS);
    JsonValue *reverse_member();
};

struct JsonString;

struct JsonPair : public JsonValue {
	std::unique_ptr<std::string> key;
    JsonPair(std::string *k);

    virtual ~JsonPair();
    virtual std::ostream &print(std::ostream &os) const;
    virtual std::ostream &printJson(std::ostream &os) const;

    virtual int toLuaObject(LS);
};

struct JsonObject : public JsonValue {
    virtual std::ostream &print(std::ostream &os) const;
    virtual std::ostream &printJson(std::ostream &os) const;
    virtual int toLuaObject(LS);
};

struct JsonArray : public JsonValue {
    virtual std::ostream &print(std::ostream &os) const;
    virtual std::ostream &printJson(std::ostream &os) const;
    virtual int toLuaObject(LS);
};

struct JsonString : public JsonValue {
	std::unique_ptr<std::string> value;
    JsonString(std::string *s);
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
    virtual std::ostream &print(std::ostream &os) const;
    virtual std::ostream &printJson(std::ostream &os) const;
    virtual int toLuaObject(LS);
};

struct JsonState {
	JsonValue *value;
    std::deque< std::unique_ptr<JsonValue> > objList;

    JsonState();

    template <class T, class... Args> T *newObject(Args &&... args) {
        static_assert(std::is_base_of<JsonValue, T>::value,
                      "must derived from JsonValue");

        objList.push_back( std::make_unique<T>( std::forward<Args>(args)... ) );

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

JsonState parse_json(FILE *);

#endif
