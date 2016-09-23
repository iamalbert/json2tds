#include <cstdio>
#include "JsonNode.h"

int main(int argc, char** argv){
    const char *filename = argv[1];

    FILE * fp = fopen(filename, "rb");
    if ( fp == NULL ){
        return -1;
    }
    puts("fopned");

    JsonState *state = parse_json(fp);

    if( state->value == NULL ){
        return -2;
    }

    puts("done");

    delete state;

    puts("close");
    fclose(fp);

}
