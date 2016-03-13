#if defined LS
#undef LS
#endif
#define LS lua_State *L

#include <functional>
#include "JsonNode.h"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

int luaopen_json2tds(LS);
int json_decode(LS);
}

struct OnExitScope {
    std::function<void()> func;
    template <class Func> OnExitScope(Func f) : func(f) {}
    ~OnExitScope() { func(); }
};

#define TOKENPASTE(x, y) x##y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define onexit(x)                                                              \
    OnExitScope TOKENPASTE2(__oes, __LINE__) {                                 \
        [&] { x; }                                                             \
    }

int json_decode(LS) {
    const char *filepath = luaL_checkstring(L, 1);

    FILE *fp = NULL;
    JsonValue *json = nullptr;

    onexit(if (fp) fclose(fp); if (json) delete json;);

    fp = fopen(filepath, "rb");

    if (fp == NULL) {
        lua_pushnil(L);
        return 1;
    }

    json = parse_json(fp);

    if (json != NULL) {
        json->toLuaObject(L);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

int luaopen_json2tds(lua_State *L) {

    lua_newtable(L);
    lua_pushcfunction(L, json_decode);
    lua_setfield(L, -2, "json_decode");

    /* Return cjson table */
    return 1;
}
