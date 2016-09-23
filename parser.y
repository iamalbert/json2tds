
%{
    #include <cstdio>
    #include <string>
    #include <memory>

    #include "JsonNode.h"
    #include "parser.hpp"
    #include "token.h"

    extern int yylex(YYSTYPE * yylval, yyscan_t  scanner );
    extern void yyerror(yyscan_t scanner, JsonState*, const char *);

%}

%define parse.error verbose
%pure-parser
%lex-param {void * scanner}
%parse-param {void * scanner} {JsonState * state}

/*
%define api.pure full 
%lex-param {void *scanner}
%parse-param {void *scanner}
*/

%union {
    JsonValue * value;

    const char* strval;
    double numval;
    int token;
}

%{
%}

%token <numval> T_NUMBER 
%token <strval> T_STRING

%token T_LEFT_CUR T_RIGHT_CUR T_LEFT_BRAK T_RIGHT_BRAK
%token T_TRUE T_FALSE T_NULL
%token T_COMMA T_COLON
%token T_END_OF_FILE
%token T_UNIDENTIFY
    

%type <value> array object elements element 
%type <value> members 


%start start

%%

start: element {
	state->value = $1;
	state->value->isRoot = true;
}

array: T_LEFT_BRAK elements T_RIGHT_BRAK { 
        //printf("arr\n");
        $$ = $2;
    }| T_LEFT_BRAK T_RIGHT_BRAK { 
        $$ = state->newObject<JsonArray>();
    }
;
elements: element {
        //printf("ele %c\n", $1->type );
        $$ = state->newObject<JsonArray>();
        $$->as<JsonArray>()->ptrVec.push_back($1);

    }| elements T_COMMA element {
        //printf("eles(%c) ele(%c)\n", $1->type, $3->type);
        $1->as<JsonArray>()->ptrVec.push_back($3);
        $$ = $1;
    }
;
object: T_LEFT_CUR members T_RIGHT_CUR { 
        //puts("obj");
        $$ = $2;
    }| T_LEFT_CUR T_RIGHT_CUR {
	    $$ = state->newObject<JsonObject>();
    }
;
members: T_STRING T_COLON element  {
        $$ = state->newObject<JsonObject>();

        $$->as<JsonObject>()->ptrTable[ state->getString($1) ] = $3 ;

        //delete (std::string*) $1;
        //printf("mem(%s:%c)\n", $1->as<JsonPair>()->key->c_str(), $1->member->type);
    }|   members T_COMMA T_STRING T_COLON element {
        $1->as<JsonObject>()->ptrTable[ state->getString($3) ] = $5;
        //delete (std::string*) $1;
        $$ = $1;
    }
;
element: T_STRING {
        $$ = state->newObject<JsonString>(state, $1 );
    }| T_NUMBER {
        $$ = state->newObject<JsonNumber>($1);
    }| T_TRUE {
        $$ = state->newObject<JsonBoolean>(true);
    }| T_FALSE {
        $$ = state->newObject<JsonBoolean>(false);
    }| T_NULL {
        $$ = state->newObject<JsonNull>();
    }| array {
        $$ = $1;
    }| object {
        $$ = $1;
    }
;

%%

//extern int yylex(YYSTYPE*);

void yyerror(yyscan_t scanner, JsonState *state, const char * err ) {
	char msg[BUFSIZ];
	sprintf(msg, "Line: %d, Token: %s, Error: %s\n", 
		yyget_lineno(scanner), yyget_text(scanner), err );
	throw JsonException(msg);
}

JsonState* parse_json( FILE * fp ){
    JsonState* state = new JsonState();
    yyscan_t scanner;

    yylex_init(&scanner);
	try{
		yyset_in( fp, scanner );
		yyparse(scanner, state);
		//state.getJsonValue()->print(std::cerr);
	}catch( std::exception & e ){
        fputs( e.what(), stderr );
        delete state;
        state = NULL;
	}
    yylex_destroy(scanner);

    return state;
}
JsonState* parse_json_string( const char * string ){
    JsonState* state = new JsonState();
    yyscan_t scanner;
    yylex_init(&scanner);
	try{
		YY_BUFFER_STATE buffer = yy_scan_string(string, scanner);
		yyset_lineno(1, scanner);
		yyparse(scanner, state);
		yy_delete_buffer(buffer, scanner);
	}catch( std::exception & e ){
        fputs( e.what(), stderr );
        delete state; state = NULL;
	}
    yylex_destroy(scanner);

    return std::move(state);
}



/*
int main(int argc, char **argv){
	for( int i = 1; i < argc; i ++ ){
		FILE * fp = fopen( argv[i], "rb" );
		JsonValue * json = parse_json(fp);
		fclose(fp);

		if( json ){
			//json->print(std::cout); std::cout << "\n";
		}else{
		}
		delete json;
	}
}

*/
