#include <cstdio>
#include "JsonNode.h"

int main(int argc, char** argv){
    const char *filename = argv[1];

    FILE * fp = fopen(filename, "rb");
    if ( fp == NULL ){
		puts("error open");
    }
    puts("fopened");

    JsonState *state = new JsonState();
    bool ok = parse_json(fp, state);

    if( not ok ){
		puts("parse error");
        delete state;
        return -2;
    }
    puts("done");

    state->free();
    delete state;

    puts("close");
    if( fp != NULL ) fclose(fp);

    return 0;
}
