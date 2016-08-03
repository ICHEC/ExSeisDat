GTEST_DIR=../googletest/googletest
GMOCK_DIR=../googletest/googlemock
GOOGLE_OBJ=gmock-all.o  gtest-all.o
LDFLAGS=$(GOOGLE_OBJ)
TEST_FRAM=-I../../googletest/googletest/include/ -I../../googletest/googlemock/include/
LOCAL_INC_FOLDER=-I../src/ $(TEST_FRAM)
OBJDIR=obj
SRC_OBJDIR=../src/obj/

#GCOMP=clang++
GCOMP=g++

include ../compiler.cfg

INC_FOLDER = -I../include $(LOCAL_INC_FOLDER)
CUSTOM_FLAGS=-DNT_LIMITS=10000000
CXXFLAGS = $(STANDARDS) $(WARNINGS) $(DEBUG) $(OPTIMISATIONS) $(INC_FOLDER) $(CUSTOM_FLAGS)

DEP=depend
SOURCES:=$(wildcard *.cc)
OBJECTS=$(filter-out ../src/obj/filesegy.o, $(wildcard ../src/obj/*.o)) $(patsubst %.cc, $(OBJDIR)/%.o, $(SOURCES))
DEPENDS:=$(patsubst %.cc, %.dep, $(SOURCES))

CURR_OBJECTS:=$(filter $(wildcard $(OBJDIR)/*.o), $(OBJECTS))
CURR_DEP:=$(wildcard $(DEP))
CURR_DEPENDS:=$(wildcard $(DEPENDS))

%.dep: %.cc
	gcc -std=c++14 -MM $< -MT $(patsubst %.cc, $(OBJDIR)/%.o, $<) $(INC_FOLDER) -o $@ 

$(OBJDIR)/%.o: %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: all
all: $(DEP) $(OBJECTS)

$(DEP): $(DEPENDS)
	cat *.dep > $(DEP)

ifeq "$(CURR_DEP)" "$(DEP)"
-include $(DEP)
endif

BIN=unittests

.PHONY: all
all: libgmock.a $(BIN) $(DEP)

libgmock.a:
	$(GCOMP) -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
    -isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
    -pthread -c ${GTEST_DIR}/src/gtest-all.cc
	$(GCOMP) -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
    -isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
    -pthread -c ${GMOCK_DIR}/src/gmock-all.cc
	ar -rv libgmock.a gtest-all.o gmock-all.o

$(BIN): $(OBJECTS)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $(OBJECTS)

.PHONY: clean
clean:
	rm -f $(OBJECTS) $(BIN) $(CURR_DEPENDS) $(CURR_DEP)

googleclean:
	rm -f libgmock.a gtest-all.o gmock-all.o

-include $(DEP)

