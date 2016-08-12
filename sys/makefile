GTEST_DIR=../googletest/googletest
GMOCK_DIR=../googletest/googlemock
GOOGLE_OBJ=gmock-all.o  gtest-all.o
LDFLAGS=$(GOOGLE_OBJ) $(TULLOW_LIBS) -L../lib/ -lcpiol
TEST_FRAM=-I../googletest/googletest/include/ -I../googletest/googlemock/include/
LOCAL_INC_FOLDER=-I../src/ $(TEST_FRAM)
OBJDIR=obj

GCOMP=g++

include ../compiler.cfg

INC_FOLDER = -I../api/ $(LOCAL_INC_FOLDER)
CUSTOM_FLAGS=-DNT_LIMITS=10000000

SOURCES=$(wildcard *.cc) $(wildcard *.c)
OBJECTS=$(patsubst %.c, $(OBJDIR)/%.co, $(patsubst %.cc, $(OBJDIR)/%.cpo, $(SOURCES)))
CPPFLAGS=$(CPPSTANDARD) $(WARNINGS) $(DEBUG) $(OPTIMISATIONS) $(INC_FOLDER) $(CUSTOM_FLAGS)
CFLAGS = $(CSTANDARD) $(WARNINGS) $(DEBUG) $(OPTIMISATIONS) $(INC_FOLDER) $(CUSTOM_FLAGS)

DEP=depend

DEPENDS:=$(patsubst %.c, %.dep, $(wildcard *.c)) $(patsubst %.cc, %.dep, $(wildcard *.cc))
CURR_DEP:=$(wildcard $(DEP))
CURR_DEPENDS:=$(wildcard $(DEPENDS))

BIN=systest

%.dep: %.c
	gcc $(CSTANDARD) -MM $< -MT $(patsubst %.c, $(OBJDIR)/%.co, $<) $(INC_FOLDER) -o $@ 

%.dep: %.cc
	g++ $(CPPSTANDARD) -MM $< -MT $(patsubst %.c, $(OBJDIR)/%.cpo, $<) $(INC_FOLDER) -o $@ 

$(OBJDIR)/%.cpo: %.cc
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(OBJDIR)/%.co: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all
all: libgmock.a $(OBJECTS) $(DEP) $(BIN)

.PHONY: clean
clean:
	rm -f obj/*.co obj/*.cpo $(BIN) $(CURR_DEPENDS) *.dep

$(BIN): $(OBJECTS) ../lib/libcpiol.so
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(OBJECTS)

googleclean:
	rm -f libgmock.a gtest-all.o gmock-all.o

libgmock.a:
	$(GCOMP) -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
	-isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
	-pthread -c ${GTEST_DIR}/src/gtest-all.cc
	$(GCOMP) -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
	-isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
	-pthread -c ${GMOCK_DIR}/src/gmock-all.cc
	ar -rv libgmock.a gtest-all.o gmock-all.o

$(DEP): $(DEPENDS)
	cat *.dep > $(DEP)

ifeq "$(CURR_DEP)" "$(DEP)"
-include $(DEP)
endif


