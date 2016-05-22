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

char * codepoint_to_char(unsigned long c, char *b ){
    if (c<0x80) *b++=c;
    else if (c<0x800) *b++=192+c/64, *b++=128+c%64;
    else if (c-0xd800u<0x800) return 0;
    else if (c<0x10000) *b++=224+c/4096, *b++=128+c/64%64, *b++=128+c%64;
    else if (c<0x110000) *b++=240+c/262144, *b++=128+c/4096%64, *b++=128+c/64%64, *b++=128+c%64;
    else {
        return 0;
    }
    return b;
}

unsigned long int hex2int(char * b ){
    return strtoul( b, NULL, 16 );
}

char * escape_str(char * first, size_t len){
    char * last = first + len;
    char * org_first = first;
    char * pos = first;

    char str[5] = {0};

    while( first != last && *first != '\0' ){
        //printf("%c\n", *first);
        if ( *first == '\\' ){
            int flag = 0;
            unsigned long codepoint = 0;
            switch( *(first+1) ){
                case 't': *pos = '\t'; break;
                case 'b': *pos = '\b'; break;
                case 'n': *pos = '\n'; break;
                case 'r': *pos = '\r'; break;
                case 'f': *pos = '\f'; break;
                case '"': *pos = '"'; break;
                case '\\': *pos = '\\'; break;
                case 'u':
                    memcpy(str, first+2, 4);
                    codepoint = hex2int(str);
                    pos = codepoint_to_char(codepoint, pos);

                    flag = 2;
                    break;
                default:
                    *pos = *first;
                    flag  = 1;
            }
            if (flag == 1) {
                pos += 1;
                first += 1;
            }else if(flag == 2){
                /* pos is moved in codepoint_to_char() */
                first += 6;
            }else {
                pos += 1;
                first += 2;
            }
        }else{
            *pos = *first;

            pos += 1;
            first += 1;
        }
    }
    *pos = '\0';
    puts(org_first);
    return org_first;
}

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
	yylval->strval = new std::string(escape_str(yytext+1, yyleng-2));
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
