BIN=blocktest

CXX = mpicxx
#CXX = clang
CXXFLAGS = -fopenmp -Wall -Wextra -mtune=native -O3 -g -std=c++14 -funroll-loops  -I../ 
LDFLAGS = 
SOURCES=$(wildcard *.cc)
OBJECTS=$(SOURCES:.cc=.o)

$(BIN): $(OBJECTS)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $(OBJECTS)

.cc.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

clean:
	rm *.o

