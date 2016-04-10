%{

#include <cstdlib>
 

#include "JsonNode.h"
#include "parser.hpp"


#define SAVE_TOKEN(n) (yylval->strval=new std::string(yytext+n,yyleng-1-n))

//#define DEBUG
#ifdef DEBUG
    #define TOKEN(t) do{ printf("tok " #t "\t: %s\n",yytext); return(t);}while(0)
#else
    #define TOKEN(t) return (t);
#endif
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
u2a             [\xC2-\xDF][\x80-\xBF]
u2b             \xE0[\xA0-\xBF][\x80-\xBF]
u3a             [\xE1-\xEC\xEE\xEF][\x80-\xBF]{2}
u3b             \xED[\x80-\x9F][\x80-\xBF]
u4a             \xF0[\x90-\xBF][\x80-\xBF]{2}
u4b             [\xF1-\xF3][\x80-\xBF]{3}
u4c             \xF4[\x80-\x8F][\x80-\xBF]{2}
UTF8LITERAL     {u2a}|{u2b}|{u3a}|{u3b}|{u4a}|{u4b}|{u4c}
CHAR            {UNESCAPEDCHAR}|{ESCAPEDCHAR}|{UNICODECHAR}|{UTF8LITERAL}
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
