CXX := clang++
CXXFLAGS := -Wall -pedantic -Wno-deprecated-register -O2 -std=c++11
CXXFLAGS := $(CXXFLAGS) -fpic

all: json2tds.so

json2tds.so: json2tds.o parser.o token.o JsonNode.o
	$(CXX) $(CXXFLAGS) -shared -o $@ $^ -ll


json2tds.o: json2tds.cpp
	$(CXX) -c $(CXXFLAGS) -I/home/wlzhuang/torch/install/include -o $@ $^

%.o: %.cpp token.h parser.hpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<


token.h: token.cpp
token.cpp: token.l parser.hpp
	flex -F --header-file=token.h -o $@ $<

parser.hpp: parser.cpp
parser.cpp: json.y
	bison -d -o $@ $<

clean:
	rm -f token.cpp token.h parser.cpp prog *.o *.so
