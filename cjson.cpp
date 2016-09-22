#include "JsonNode.h"

#define STRINGIFY(x) #x
#define XSTRINGIFY(x) STRINGIFY(x)

#define PACKAGE_NAME cjson
#define PACKAGE_NAME_(x) PACKAGE_NAME
#define PACKAGE_NAME_STR XSTRINGIFY(PACKAGE_NAME)

const char * const PACKAGE = PACKAGE_NAME_STR;

#define PASTE(x,y) x ## y
#define XPASTE(x,y) PASTE(x,y)
#define METHOD(x) XPASTE( PACKAGE_NAME, _ ## x)

#define METHOD_DECLARE(x) extern "C" int METHOD(x) (LS)

extern "C" {
	int luaopen_cjson(LS);
}

METHOD_DECLARE(__len){
	JsonValue *self = *(JsonValue**)luaL_checkudata(L, 1, PACKAGE_NAME_STR);
	return self->luaLen(L);
}

METHOD_DECLARE(loads){
	const char *string = luaL_checkstring(L, 1);
	JsonState *state = parse_json_string(string);

	if( state->value == NULL ){
        luaL_error(L, "parse error, not a valid JSON");
		return 0;
	}

	JsonValue** self = (JsonValue**) lua_newuserdata(L, sizeof(JsonValue*) );
	luaL_getmetatable(L, PACKAGE_NAME_STR);
	lua_setmetatable(L, -2);
	
	*self = state->value;

	return 1;
}

METHOD_DECLARE(__gc){
	JsonValue *self = *(JsonValue**)luaL_checkudata(L, 1, PACKAGE_NAME_STR);

	delete self->root;

	return 1;
}

METHOD_DECLARE(__index){
	JsonValue *self = *(JsonValue**)luaL_checkudata(L, 1, PACKAGE_NAME_STR);

	return self->luaGet(L);
}

bool JsonValue::isBaseType() const {
    switch(type){
        case 'n': 
        case 'b': 
        case 's': 
			return true;
		case 'o':
		case 'a':
			return false;
		default:
			return false;
	}
}

int JsonValue::luaLen(LS){

    switch(type){
        case 'n': 
        case 'b': 
        case 's': 
			luaL_error(L, "base type has no length");
			return 0;
		break;

        case 'o': 
			lua_pushinteger(L, as<JsonObject>()->ptrTable.size() );
			return 1;
		break;
        case 'a': {
			lua_pushinteger(L, as<JsonArray>()->ptrVec.size() );
			return 1;
        } break;
        case 'p': 
        default:
            luaL_error(L, "unknown type: %c", type);
            return 0;
    }
    luaL_error(L, "unknown");
    return 0;
}
int JsonValue::luaGet(LS){

    switch(type){
        case 'n': 
        case 'b': 
        case 's': 
		{
			luaL_error(L, "base type cannot be indexed");
        } break;

        case 'o': {
			size_t len;
			const char *cstr = luaL_checklstring(L, 2, &len);
			std::string key{cstr};

            JsonObject * self = as<JsonObject>();
            if ( self->ptrTable.count(key) == 0 ){
                luaL_error(L, "no such key `%s'", cstr);
                return 0;
            }else{
                return self->ptrTable[key]->luaGet(L);
            }
		} break;
        case 'a': {
            int index = luaL_checkint(L, 2 ) - LUA_INDEX_BASE;
            JsonArray* arr = (JsonArray*) this;
            if ( index < 0 || index >= arr->ptrVec.size() ){
                luaL_error(L, "out of range: [1, %d], %d given",
                    arr->ptrVec.size(), index + 1
                );
                return 0;
            }else{
				JsonValue * ele = arr->ptrVec[index];
				if( ele->isBaseType() ){
					return ele->toLuaObject(L);
				}else{
					JsonValue** p = (JsonValue**) lua_newuserdata(L, sizeof(JsonValue*) );
					printf("ptr: %p\n", ele->root );
					*p = ele;
					luaL_getmetatable(L, PACKAGE_NAME_STR);
					lua_setmetatable(L, -2);
					return 1;
				}
            }
        } break;
        case 'p': 
        default:
            luaL_error(L, "unknown type: %c", type);
            return 0;
    }
    luaL_error(L, "unknown");
    return 0;
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
	ADD_METHOD(__index);
	ADD_METHOD(loads);

	return 1;
}
