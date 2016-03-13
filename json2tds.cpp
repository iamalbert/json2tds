#if defined LS
    #undef LS
#endif
#define LS lua_State *L

#include "JsonNode.h"

extern "C" {
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>

    int luaopen_json2tds(LS);
    int json_decode(LS);
}


int json_decode(LS)
{
    const char * filepath = luaL_checkstring(L, 1);

    FILE *fp = NULL;
    JsonValue * json = nullptr;
    
    fp = fopen(filepath, "rb");

    if( fp == NULL ){
        lua_pushnil(L);
    }else{
        json = parse_json(fp);

        if( json != NULL ){
            json->printJson(std::cout);
            std::cout << "\n";

            delete json;
            json = nullptr;
            lua_pushnumber(L, 190);
        }else{
            lua_pushnumber(L, 10);
        }
        fclose(fp);
    }
    return 1;
}


int luaopen_json2tds(lua_State *L)
{

    lua_newtable(L);
    lua_pushcfunction(L, json_decode);
    lua_setfield(L, -2, "json_decode");

    /* Return cjson table */
    return 1;
}
