CXX := clang++
CXXFLAGS := -g

all: prog

prog: parser.o token.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -ll

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

token.cpp: token.l parser.hpp
	flex -F -o $@ $<

parser.hpp: parser.cpp
parser.cpp: json.y
	bison -d -o $@ $<

clean:
	rm -f token.cpp token.h parser.cpp prog *.o
