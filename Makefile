CXX := g++


LUA_HOME := $(realpath $(dir $(shell which th))../)

LUA_INCDIR ?= $(LUA_HOME)/include
LUA_LIBDIR ?= $(LUA_HOME)/lib

CXXFLAGS ?= -Wall -pedantic -Wno-deprecated-register -O2 -std=c++14 -I$(LUA_HOME)/include

all: json2tds.so #testcpp

install: json2tds.so
	cp $^ $(LIBDIR)


json2tds.so: json2tds.cpp parser.o token.o JsonNode.o
	$(CXX) $(CXXFLAGS) -fPIC -shared -o $@ $^ -ll
cjson.so: cjson.cpp parser.o token.o JsonNode.o
	$(CXX) $(CXXFLAGS) -fPIC -shared -o $@ $^ -ll

%.o: %.cpp token.h parser.hpp
	$(CXX) -fPIC -shared -c $(CXXFLAGS) -o $@ $<

testcpp: testcpp.o parser.o token.o JsonNode.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -L$(LUA_HOME)/lib -ll -lluajit

json2tds.o: json2tds.cpp
	$(CXX) -fPIC -shared -c $(CXXFLAGS)  -o $@ $^


token.h: token.cpp
token.cpp: token.l parser.hpp
	flex -F -8 --header-file=token.h -o $@ $<

parser.hpp: parser.cpp
parser.cpp: parser.y
	bison -d -o $@ $<

clean:
	rm -f token.cpp token.h parser.cpp prog *.o *.so testcpp
