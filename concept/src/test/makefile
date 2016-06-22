include ../compiler.cfg

BIN1=blocktest
BIN2=filetest
BIN3=opstest
BIN=$(BIN1) $(BIN2) $(BIN3)

SOURCES=$(wildcard *.cc)
OBJECTS=$(patsubst %.cc, %.o, $(SOURCES))

OBJ1=blocktest.o ../obj/blockmpiio.o
OBJ2=filetest.o segy.o ../obj/*.o
OBJ3=opstest.o segy.o ../obj/*.o

all: $(BIN1) $(BIN2) $(BIN3)

$(BIN1): $(OBJ1)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $(OBJ1)

$(BIN2): $(OBJ2)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $(OBJ2)

$(BIN3): $(OBJ3)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $(OBJ3)

.cc.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

clean:
	rm $(OBJECTS) $(BIN)

