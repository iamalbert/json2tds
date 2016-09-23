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

    printf("size : %lu\n", state->objList.size() );
    *self = state->value;

	// for( auto & v : state->strPool ){ std::cout << "p:" << v << "\n"; }


    return 1;
}

METHOD_DECLARE(load){
    const char *filename = luaL_checkstring(L, 1);

    FILE * fp = fopen(filename, "rb");
    if ( fp == NULL ){
        luaL_error(L, "file not found `%s'", filename);
        return 0;
    }

    JsonState *state = parse_json(fp);

    if( state->value == NULL ){
        luaL_error(L, "parse error, not a valid JSON file");
        return 0;
    }

    JsonValue** self = (JsonValue**) lua_newuserdata(L, sizeof(JsonValue*) );
    luaL_getmetatable(L, PACKAGE_NAME_STR);
    lua_setmetatable(L, -2);
    
    printf("size : %lu\n", state->objList.size() );

	/*
    for( auto & val : state->objList ){
        if ( val->type == 'p' ){
            delete val.release();
        }
    }
	*/

    fclose(fp);

    *self = state->value;

    return 1;
}

METHOD_DECLARE(__gc){
    JsonValue *self = *(JsonValue**)luaL_checkudata(L, 1, PACKAGE_NAME_STR);

	if ( self->isRoot ){
		delete self->root;
	}

    return 1;
}

METHOD_DECLARE(__index){
    JsonValue *self = *(JsonValue**)luaL_checkudata(L, 1, PACKAGE_NAME_STR);

    return self->luaGet(L);
}
METHOD_DECLARE(type){
    JsonValue *self = *(JsonValue**)luaL_checkudata(L, 1, PACKAGE_NAME_STR);

    lua_pushstring(L, self->typeString() ) ; 

    return 1;
}
METHOD_DECLARE(totable){
    JsonValue *self = *(JsonValue**)luaL_checkudata(L, 1, PACKAGE_NAME_STR);
    return self->asLuaObject(L);
}
METHOD_DECLARE(keys){
    JsonValue *self = *(JsonValue**)luaL_checkudata(L, 1, PACKAGE_NAME_STR);
    if( self->type != 'o' ){
        luaL_error(L, "expecting JsonObject, %s given", self->typeString() );
    }

    lua_newtable(L);
    int i = 1;
    for ( auto & kv : self->as<JsonObject>()->ptrTable ){
        lua_pushinteger(L, i);
        lua_pushstring(L, kv.first->c_str() );
        lua_settable(L, -3);
        i+=1;
    }

    return 1;
}


bool JsonValue::isBaseType() const {
    switch(type){
        case 'n': 
        case 'b': 
        case 's': 
        case 'x':
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
        case 'x':
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

	//puts("called");
    switch(type){
        case 'n': 
        case 'b': 
        case 's': 
        case 'x': 
        {
            luaL_error(L, "base type cannot be indexed");
        } break;

        case 'o': {
            size_t len;
            const char *cstr = luaL_checklstring(L, 2, &len);
			//printf("cst: %s\n",  cstr );

            const std::string *  key = root->getString(cstr);

			//printf("key: %p\n", key);

            JsonObject * self = as<JsonObject>();

            if ( self->ptrTable.count(key) == 0 ){
                luaL_error(L, "no such key `%s'", cstr);
                return 0;
            }else{
				//puts("found");
                JsonValue * ele = self->ptrTable[key];
                if( ele->isBaseType() ){
                    return ele->toLuaObject(L);
                }else{
                    JsonValue** p = (JsonValue**) lua_newuserdata(L, sizeof(JsonValue*) );
                    *p = ele;
                    luaL_getmetatable(L, PACKAGE_NAME_STR);
                    lua_setmetatable(L, -2);
                    return 1;
                }
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

extern "C"
int luaopen_libcjson(LS){

    int ok;

    lua_newtable(L);

    ok = luaL_newmetatable(L, PACKAGE_NAME_STR);
    if( not ok ){
        luaL_error(L, "metatable `json2tds' already exists");
    }

#define ADD_METHOD(name) lua_pushcfunction(L, METHOD(name) ); lua_setfield(L, -2, #name);
    ADD_METHOD(__len);
    ADD_METHOD(__gc);
    ADD_METHOD(__index);

    lua_setfield(L, -2, "mt");

    ADD_METHOD(type);
    ADD_METHOD(loads);
    ADD_METHOD(load);
    ADD_METHOD(totable);
    ADD_METHOD(keys);

    return 1;
}
