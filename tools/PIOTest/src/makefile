BIN=../run/piotest

CXX = mpicxx
CXXFLAGS = -mtune=native -O3 -std=c++14 -funroll-loops -I../include 
LDFLAGS = 
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

$(BIN): $(OBJECTS)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $(OBJECTS)

.cpp.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

test.cpp: test.h

clean:
	rm *.o

