CXX := g++
LUA_HOME:=/home/wlzhuang/torch/install

CXXFLAGS := -Wall -pedantic -Wno-deprecated-register -O2 -std=c++11
CXXFLAGS := $(CXXFLAGS) -I$(LUA_HOME)/include

all: json2tds.so testcpp

json2tds.so: json2tds.cpp parser.o token.o JsonNode.o
	$(CXX) $(CXXFLAGS) -fPIC -shared -o $@ $^ -ll

testcpp: testcpp.o parser.o token.o JsonNode.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -L$(LUA_HOME)/lib -ll -lluajit

json2tds.o: json2tds.cpp
	$(CXX) -fPIC -shared -c $(CXXFLAGS)  -o $@ $^

%.o: %.cpp token.h parser.hpp
	$(CXX) -fPIC -shared -c $(CXXFLAGS) -o $@ $<


token.h: token.cpp
token.cpp: token.l parser.hpp
	flex -F --header-file=token.h -o $@ $<

parser.hpp: parser.cpp
parser.cpp: parser.y
	bison -d -o $@ $<

clean:
	rm -f token.cpp token.h parser.cpp prog *.o *.so
