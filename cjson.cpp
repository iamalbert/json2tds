#if defined LS
	#undef LS
#endif
#define LS lua_State *L

extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>

	int luaopen_cjson(LS);
}

#define STRINGIFY(x) #x
#define XSTRINGIFY(x) STRINGIFY(x)

#define PACKAGE_NAME cjson
#define PACKAGE_NAME_(x) PACKAGE_NAME
#define PACKAGE_NAME_STR XSTRINGIFY(PACKAGE_NAME)

const char * const PACKAGE = PACKAGE_NAME_STR;

struct JsonValue {
	int value;
	JsonValue(int v): value(v) {}

	~JsonValue(){
		puts("deleted");
	}
};

#define PASTE(x,y) x ## y
#define XPASTE(x,y) PASTE(x,y)
#define METHOD(x) XPASTE( PACKAGE_NAME, _ ## x)

#define METHOD_DECLARE(x) int METHOD(x) (LS)

METHOD_DECLARE(__len){
	luaL_checkudata(L, 1, PACKAGE_NAME_STR);
	lua_pushnumber(L, 6692);
	return 1;
}

METHOD_DECLARE(new){
	JsonValue** addr = (JsonValue**) lua_newuserdata(L, sizeof(JsonValue*) );
	luaL_getmetatable(L, PACKAGE_NAME_STR);
	lua_setmetatable(L, -2);

	*addr = new JsonValue(10000);

	return 1;
}

METHOD_DECLARE(getVal){
	JsonValue *addr = *(JsonValue**)luaL_checkudata(L, 1, PACKAGE_NAME_STR);

	lua_pushnumber(L, addr->value);
	return 1;
}

METHOD_DECLARE(__gc){
	JsonValue *addr = *(JsonValue**)luaL_checkudata(L, 1, PACKAGE_NAME_STR);

	delete addr;

	return 1;
}

int luaopen_cjson(LS){

	int ok;

	ok = luaL_newmetatable(L, PACKAGE_NAME_STR);
	if( not ok ){
		luaL_error(L, "metatable `json2tds' already exists");
	}

#define ADD_METHOD(name) lua_pushcfunction(L, METHOD(name) ); lua_setfield(L, -2, #name);

	ADD_METHOD(__len);
	ADD_METHOD(__gc);
	ADD_METHOD(new);

	lua_newtable(L);
	ADD_METHOD(getVal);
    lua_setfield(L, -2, "__index");

	return 1;
}
