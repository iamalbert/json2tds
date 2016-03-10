CXX := g++
CXXFLAGS := -Wall -pedantic -Wno-deprecated-register -O2 -std=c++11

all: prog

prog: parser.o token.o JsonNode.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -ll

%.o: %.cpp token.h parser.hpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<


token.h: token.cpp
token.cpp: token.l parser.hpp
	flex -F --header-file=token.h -o $@ $<

parser.hpp: parser.cpp
parser.cpp: json.y
	bison -d -o $@ $<

clean:
	rm -f token.cpp token.h parser.cpp prog *.o
