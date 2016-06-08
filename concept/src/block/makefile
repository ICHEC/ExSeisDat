BIN=blocktest

CXX = mpicxx
CXXFLAGS = -mtune=native -O3 -std=c++14 -funroll-loops # -I../include 
LDFLAGS = 
SOURCES=$(wildcard *.cc)
OBJECTS=$(SOURCES:.cc=.o)

$(BIN): $(OBJECTS)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $(OBJECTS)

.cc.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

clean:
	rm *.o

