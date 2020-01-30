BASE = kbw

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)

BISON = bison
CXX = g++
FLEX = flex
BISONFLAGS = -v

all: $(BASE)

%.cpp %.hpp : %.yy
	$(BISON) $(BISONFLAGS) -o$*.cpp $<

%.cpp: %.ll
	$(FLEX) $(FLEXFLAGS) -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BASE): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(OBJ) $(BASE)
