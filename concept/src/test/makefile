include ../compiler.cfg

BIN1=datatest
BIN2=filetest
BIN3=opstest
BIN=$(BIN1) $(BIN2) $(BIN3)

SOURCES=$(wildcard *.cc)
OBJECTS=$(patsubst %.cc, %.o, $(SOURCES))

OBJ1=datatest.o ../obj/datampiio.o
OBJ2=filetest.o segy.o ../obj/*.o
OBJ3=opstest.o segy.o ../obj/*.o

DEP:=depend
DEPENDS:=$(patsubst %.cc, %.dep, $(SOURCES))
CURR_DEP:=$(wildcard $(DEP))

.PHONY: all
all: $(BIN1) $(BIN2) $(BIN3) $(DEP)

$(BIN1): $(OBJ1)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $(OBJ1)

$(BIN2): $(OBJ2)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $(OBJ2)

$(BIN3): $(OBJ3)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $(OBJ3)

$(DEP): $(DEPENDS)
	cat *.dep > $(DEP)

.cc.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

%.dep: %.cc
	gcc -std=c++14 -MM $< -MT $(patsubst %.cc, $(OBJDIR)/%.o, $<) $(INC_FOLDER) -o $@ 

.PHONY: clean
clean:
	rm -f $(OBJECTS) $(BIN) $(DEPENDS) $(CURR_DEP)

-include $(DEP)

