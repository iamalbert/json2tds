#include "JsonNode.h"

#define STRINGIFY(x) #x
#define XSTRINGIFY(x) STRINGIFY(x)

#define PACKAGE_NAME cjson
#define PACKAGE_NAME_(x) PACKAGE_NAME
#define PACKAGE_NAME_STR XSTRINGIFY(PACKAGE_NAME)

const char * const PACKAGE = PACKAGE_NAME_STR;

struct JsonValue2 {
	int value;
	JsonValue2(int v): value(v) {}

	~JsonValue2(){
		puts("deleted");
	}
};

#define PASTE(x,y) x ## y
#define XPASTE(x,y) PASTE(x,y)
#define METHOD(x) XPASTE( PACKAGE_NAME, _ ## x)

#define METHOD_DECLARE(x) extern "C" int METHOD(x) (LS)

extern "C" {
	int luaopen_cjson(LS);
}

METHOD_DECLARE(__len){
	luaL_checkudata(L, 1, PACKAGE_NAME_STR);
	lua_pushnumber(L, 6692);
	return 1;
}

METHOD_DECLARE(new){
	JsonValue2** self = (JsonValue2**) lua_newuserdata(L, sizeof(JsonValue2*) );
	luaL_getmetatable(L, PACKAGE_NAME_STR);
	lua_setmetatable(L, -2);

	*self = new JsonValue2(10000);

	return 1;
}

METHOD_DECLARE(loads){
	const char *string = luaL_checkstring(L, 1);
	JsonState state = parse_json_string(string);

	if( state.value == NULL ){
        luaL_error(L, "parse error, not a valid JSON");
		return 0;
	}
	return METHOD(new)(L);
}

METHOD_DECLARE(__gc){
	JsonValue2 *self = *(JsonValue2**)luaL_checkudata(L, 1, PACKAGE_NAME_STR);

	delete self;

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
	ADD_METHOD(loads);

	lua_newtable(L);
    lua_setfield(L, -2, "__index");

	return 1;
}
