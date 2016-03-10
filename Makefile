CXX := clang++
CXXFLAGS := -g

all: prog

prog: parser.cpp token.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ -ll

token.cpp: token.l parser.hpp
	flex -F --header-file=token.h -o $@ $<

parser.hpp: parser.cpp
parser.cpp: json.y
	bison -d -o $@ $<

clean:
	rm -f token.cpp token.h parser.cpp prog *.o
