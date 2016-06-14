BIN1=blocktest
BIN2=filetest

include ../compiler.cfg
LDFLAGS= 
OBJ1=blocktest.o ../obj/mpiio.o
OBJ2=filetest.o ../obj/mpiio.o

all: blocktest filetest

blocktest: $(OBJ1)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $(OBJ1)

filetest: $(OBJ2)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $(OBJ2)

.cc.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

clean:
	rm *.o

