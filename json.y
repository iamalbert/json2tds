
%{
    #include <iostream>
    #include <cstdio>
    #include <string>

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

    std::string * strval;
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
    

%type <value> array object elements element members pair collection


%start start

%%

start: collection 

collection: array  { 
        state->value = $1;
    }| object {
        state->value = $1;
    }
;
array: T_LEFT_BRAK elements T_RIGHT_BRAK { 
        $$ = state->newObject<JsonArray>();
        $$->member = $2;
    }| T_LEFT_BRAK T_RIGHT_BRAK { 
        $$ = state->newObject<JsonArray>();
    }
;
elements: element {
		$$ = $1;
    }| element T_COMMA elements {
		$1->next = $3;
		$$= $1;
    }
;
element: T_STRING {
        //std::cout << "STRING:" << *$1.strval << "\n";
        $$ = state->newObject<JsonString>($1);
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
object: T_LEFT_CUR T_RIGHT_CUR {
		$$ = state->newObject<JsonObject>();
      }
      | T_LEFT_CUR members T_RIGHT_CUR { 
		$$ = state->newObject<JsonObject>();
        $$->member = $2;
      }
;
members: pair T_COMMA members {
            $1->next = $3;
            $$ = $1;
       }
       | pair {
            $$ = $1;
       }
;
pair: T_STRING T_COLON element {
    $$ = state->newObject<JsonPair>($1);
	$$->member =  $3;
}
;

%%

//extern int yylex(YYSTYPE*);

void yyerror(yyscan_t scanner, JsonState *state, const char * err ) {
	char msg[BUFSIZ];
	sprintf(msg, "Line: %d, Token: %s, Error: %s\n", 
		yyget_lineno(scanner), yyget_text(scanner), err );
	throw JsonException(msg);
};


JsonValue* parse_json( FILE * fp ){
    JsonState state;
    yyscan_t scanner;

    yylex_init(&scanner);
	try{
		yyset_in( fp, scanner );
		yyparse(scanner, &state);
		//state.getJsonValue()->print(std::cerr);
	}catch( std::exception & e ){
		std::cerr << e.what();
		state.free();
	}
    yylex_destroy(scanner);

	return state.getJsonValue();
}



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
};

