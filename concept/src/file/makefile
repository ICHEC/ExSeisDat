BIN=blocktest

include ../compiler.cfg
LDFLAGS = 
SOURCES=$(wildcard *.cc)
OBJECTS=$(SOURCES:.cc=.o)

$(BIN): $(OBJECTS)

.cc.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

clean:
	rm *.o

