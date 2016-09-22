CXX := g++


LUA_HOME := $(realpath $(dir $(shell which th))../)

LUA_INCDIR ?= $(LUA_HOME)/include
LUA_LIBDIR ?= $(LUA_HOME)/lib

CXXFLAGS ?= -Wall -pedantic -Wno-deprecated-register -O2 -std=c++14 -I$(LUA_HOME)/include

all: libcjson.so

install: libcjson.so
	cp $^ $(LIBDIR)


libcjson.so: cjson.cpp parser.o token.o JsonNode.o
	$(CXX) $(CXXFLAGS) -fPIC -shared -o $@ $^ -ll

%.o: %.cpp token.h parser.hpp
	$(CXX) -fPIC -shared -c $(CXXFLAGS) -o $@ $<

token.h: token.cpp
token.cpp: token.l parser.hpp
	flex -F -8 --header-file=token.h -o $@ $<

parser.hpp: parser.cpp
parser.cpp: parser.y
	bison -d -o $@ $<

clean:
	rm -f token.cpp token.h parser.cpp *.o *.so
