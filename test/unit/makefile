include compiler.cfg

BIN1=unittests
BIN=$(BIN1) $(BIN2) $(BIN3)

SOURCES=$(wildcard *.cc)
OBJECTS=$(patsubst %.cc, %.o, $(SOURCES))

DEP:=depend
DEPENDS:=$(patsubst %.cc, %.dep, $(SOURCES))
CURR_DEP:=$(wildcard $(DEP))

.PHONY: all
all: $(BIN) $(DEP)

$(BIN): $(OBJECTS)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $(OBJECTS) $(wildcard ../../concept/src/obj/*.o) ../../concept/src/test/segy.o

$(DEP): $(DEPENDS)
	cat *.dep > $(DEP)

.cc.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

%.dep: %.cc
	gcc -std=c++14 -MM $< -MT $(patsubst %.cc, $(OBJDIR)/%.o, $<) $(INC_FOLDER) -o $@ 

googletest:
	g++ -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
    -isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
    -pthread -c ${GTEST_DIR}/src/gtest-all.cc
	g++ -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
    -isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
    -pthread -c ${GMOCK_DIR}/src/gmock-all.cc
	ar -rv libgmock.a gtest-all.o gmock-all.o

.PHONY: clean
clean:
	rm -f $(OBJECTS) $(BIN) $(DEPENDS) $(CURR_DEP)

-include $(DEP)

