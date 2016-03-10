%{

#include <cstdlib>

#include "JsonNode.h"
#include "parser.hpp"


#define SAVE_TOKEN(n) (yylval->strval=new std::string(yytext+n,yyleng-1-n))
#define TOKEN(t) do{ /*puts("token: "  #t);*/ return(t);}while(0)
//(yylval.token = t)


%}

%option yylineno
%option reentrant
%option bison-bridge

DIGIT1to9   [1-9]
DIGIT       [0-9]
DIGITS      {DIGIT}+

INT         {DIGIT}|{DIGIT1to9}{DIGITS}|-{DIGIT}|-{DIGIT1to9}{DIGITS}
FRAC        [.]{DIGITS}
EXP         {E}{DIGITS}
E           [eE][+-]?

NUMBER      {INT}|{INT}{FRAC}|{INT}{EXP}|{INT}{FRAC}{EXP}

DBL_QUOTE       ["]

HEX_DIGIT       [0-9A-Fa-f]
UNESCAPEDCHAR   [ -!#-\[\]-~]
ESCAPEDCHAR     \\["\\bfnrt]
UNICODECHAR     \\u{HEX_DIGIT}{HEX_DIGIT}{HEX_DIGIT}{HEX_DIGIT}
CHAR            {UNESCAPEDCHAR}|{ESCAPEDCHAR}|{UNICODECHAR}
CHARS           {CHAR}+

%%

{DBL_QUOTE}{DBL_QUOTE} |
{DBL_QUOTE}{CHARS}{DBL_QUOTE} { 
	yylval->strval = new std::string(yytext+1, yyleng-2);
    TOKEN(T_STRING);
};

{NUMBER} { 
	yylval->numval = atof(yytext);
	TOKEN(T_NUMBER); 
};

true     { TOKEN(T_TRUE); };
false    { TOKEN(T_FALSE); };
null     { TOKEN(T_NULL); };

"{"      { TOKEN(T_LEFT_CUR); };
"}"      { TOKEN(T_RIGHT_CUR); };

"["      { TOKEN(T_LEFT_BRAK); };
"]"      { TOKEN(T_RIGHT_BRAK); };

","      { TOKEN(T_COMMA); };
":"      { TOKEN(T_COLON); };

[ \t\n\r]+ /* ignore whitespace */;

.       { TOKEN(T_UNIDENTIFY); }

%%


