BIN1=blocktest
BIN2=filetest

include ../compiler.cfg
LDFLAGS= 
OBJ1=blocktest.o ../obj/blockmpiio.o
OBJ2=filetest.o segy.o ../obj/file.o ../obj/filesegy.o ../obj/blockmpiio.o ../obj/objsegy.o

all: blocktest filetest

blocktest: $(OBJ1)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $(OBJ1)

filetest: $(OBJ2)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $(OBJ2)

.cc.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

clean:
	rm *.o

