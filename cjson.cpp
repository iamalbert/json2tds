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

struct JsonData {

    JsonState * state;
    JsonValue * value;

    JsonData(JsonState *s, JsonValue *v) : state(s), value(v) {}

    bool isRoot() const {
        return value == state->value;
    }
    void free() {
        if( isRoot() ){
            state->free();
        }
    }
};

void create(LS, JsonState* state, JsonValue * value ){

    JsonData* self = (JsonData*) lua_newuserdata(L, sizeof(JsonData) );

    new(self) JsonData(state, value);

    luaL_getmetatable(L, PACKAGE_NAME_STR);
    lua_setmetatable(L, -2);

}


METHOD_DECLARE(loads){
    const char *string = luaL_checkstring(L, 1);
    JsonState *state = new JsonState();
    
    bool ok = parse_json_string(string, state);

    if(not ok){
        luaL_error(L, "parse error, not a valid JSON");
        return 0;
    }

    create(L, state, state->value);
    return 1;
}

METHOD_DECLARE(load){
    const char *filename = luaL_checkstring(L, 1);

    FILE * fp = fopen(filename, "rb");
    if ( fp == NULL ){
        luaL_error(L, "file not found `%s'", filename);
        return 0;
    }
    JsonState *state = new JsonState();

    bool ok = parse_json(fp, state);

    fclose(fp);

    if( not ok ){
        luaL_error(L, "parse error, not a valid JSON file");
        return 0;
    }

    create(L, state, state->value);
    return 1;
}

METHOD_DECLARE(__len){
    JsonData *self = (JsonData*) luaL_checkudata(L, 1, PACKAGE_NAME_STR);
    return self->value->luaLen(L);
}
METHOD_DECLARE(__gc){
    JsonData *self = (JsonData*) luaL_checkudata(L, 1, PACKAGE_NAME_STR);

	if ( self && self->isRoot() ){
        self->free();
        self->~JsonData();
        delete self->state;
	}

    return 0;
}

METHOD_DECLARE(__index){
    JsonData *self = (JsonData*) luaL_checkudata(L, 1, PACKAGE_NAME_STR);

    return self->value->luaGet(L, self->state);
}
METHOD_DECLARE(type){
    JsonData *self = (JsonData*) luaL_checkudata(L, 1, PACKAGE_NAME_STR);

    lua_pushstring(L, self->value->typeString() ) ; 

    return 1;
}
METHOD_DECLARE(totable){
    JsonData *self = (JsonData*) luaL_checkudata(L, 1, PACKAGE_NAME_STR);
    return self->value->toLuaObject(L);
}
METHOD_DECLARE(keys){
    JsonData *self = (JsonData*) luaL_checkudata(L, 1, PACKAGE_NAME_STR);
    JsonValue * value = self->value;

    if( value->type != 'o' ){
        luaL_error(L, "expecting JsonObject, %s given", value->typeString() );
    }

    auto & table = value->as<JsonObject>()->ptrTable ;
    int i = table.size();

    lua_newtable(L);
    for ( auto & kv : table){
        lua_pushinteger(L, i);
        lua_pushstring(L, kv.first );
        lua_settable(L, -3);
        i -= 1;
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
int JsonValue::luaGet(LS, JsonState * state){

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

            const char *  key = state->searchString(cstr);

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
                    create(L, state, ele );
                    return 1;
                }
            }
        } break;
        case 'a': {
            int index = luaL_checkint(L, 2 ) - LUA_INDEX_BASE;
            JsonArray* arr = (JsonArray*) this;
            if ( index < 0 || (size_t) index >= arr->ptrVec.size() ){
                luaL_error(L, "out of range: [1, %d], %d given",
                    arr->ptrVec.size(), index + 1
                );
                return 0;
            }else{
                JsonValue * ele = arr->ptrVec[index];
                if( ele->isBaseType() ){
                    return ele->toLuaObject(L);
                }else{
                    create(L, state, ele );
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

std::unique_ptr<std::string[]> a;
METHOD_DECLARE(testalloc){
    printf("%d\n", (bool) a);
    a.reset( new std::string[100000000] ) ;
    printf("%d\n", (bool) a);
    return 0;
}
METHOD_DECLARE(testfree){
    printf("%d\n", (bool) a);
    a.reset( nullptr );
    printf("%d\n", (bool) a);
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

    ADD_METHOD(testalloc);
    ADD_METHOD(testfree);

    return 1;
}
