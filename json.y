
%{
	#include <iostream>
    #include <cstdio>
    #include <string>

    #include "JsonNode.h"
	//#include "token.h"
	#include "parser.hpp"


	JsonValue * json;

	extern int yylex();
	extern void yyerror(const char *);
%}

%define parse.error verbose

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

%token <numval> T_NUMBER 
%token <strval> T_STRING

%token T_LEFT_CUR T_RIGHT_CUR T_LEFT_BRAK T_RIGHT_BRAK
%token T_TRUE T_FALSE T_NULL
%token T_COMMA T_COLON
%token T_END_OF_FILE
    

%type <value> array object elements element members pair collection


%start start

%%

start: collection 

collection: array  { 
		json = $1;
	}| object {
		json = $1;
	}
;
array: T_LEFT_BRAK elements T_RIGHT_BRAK { 
		$$ = new JsonArray();
		$$->member = $2;
	}| T_LEFT_BRAK T_RIGHT_BRAK { 
		$$ = new JsonArray();
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
		$$ = new JsonString($1);
	}| T_NUMBER {
		$$ = new JsonNumber($1);
    }| T_TRUE {
		$$ = new JsonBoolean(true);
	}| T_FALSE {
		$$ = new JsonBoolean(false);
	}| T_NULL {
		$$ = new JsonNull();
	}| array {
		$$ = $1;
	}| object {
		$$ = $1;
	}
;
object: T_LEFT_CUR T_RIGHT_CUR {
		$$= new JsonObject();
	  }
	  | T_LEFT_CUR members T_RIGHT_CUR { 
		$$= new JsonObject();
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
	$$ = new JsonPair($1, $3);
}
;

%%

//extern int yylex(YYSTYPE*);

//void yyerror(yyscan_t scaner, const char * err) {
void yyerror(const char * err) {
	fprintf(stderr, "Error: %s\n", err );
};


int main(){
	JsonState state;
    //return yyparse( (void*) &state );

	/*
	yyscan_t scanner;

	yylex_init(&scanner);
	yyparse(scanner);
	yylex_destroy(scanner);
	*/
	yyparse();


	json->print(std::cout);
	std::cout << "\n";
	delete json;
	//std::cout << *json << "\n";
};

