BASE = kbw

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o src/parser.o src/scanner.o)

BISON = bison
CXX = g++
FLEX = flex
BISONFLAGS = -v
CXXFLAGS = -g -Wall -Werror
CXXLINK = -lboost_program_options -lboost_serialization

all: $(BASE)

%.cpp %.hpp : %.yy
	$(BISON) $(BISONFLAGS) -o$*.cpp $<

%.cpp: %.ll
	$(FLEX) $(FLEXFLAGS) -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BASE): $(OBJ) src/parser.o
	$(CXX) $(CXXFLAGS) $(CXXLINK) -o $@ $^

$(OBJ): src/parser.hpp
src/parser.o: src/parser.hpp
src/scanner.o: src/parser.hpp

install: $(BASE)
	cp $(BASE) ~/bin


clean:
	rm -f $(OBJ) $(BASE) src/location.hh src/parser.cpp src/parser.hpp src/parser.output
