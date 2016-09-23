#include <cstdio>
#include "JsonNode.h"

int main(int argc, char** argv){
    const char *filename = argv[1];

    FILE * fp = fopen(filename, "rb");
    if ( fp == NULL ){
		puts("error open");
    }
    puts("fopened");

    JsonState *state = parse_json(fp);

    if( state->value == NULL ){
		puts("parse error");
        return -2;
    }
    puts("done");

    delete state;

    puts("close");
    if( fp != NULL ) fclose(fp);
    return 0;
}
