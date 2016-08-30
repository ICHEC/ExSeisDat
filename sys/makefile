CLDFLAGS=$(TULLOW_LIBS) -L../lib/ -lcpiol -lpiol
CPPLDFLAGS=$(TULLOW_LIBS) -L../lib/ -lpiol
LOCAL_INC_FOLDER=-I../src/
OBJDIR=obj

GCOMP=g++

include ../compiler.cfg

CPPINC_FOLDER = -I../include/ $(LOCAL_INC_FOLDER)
CINC_FOLDER = -I../api/ $(LOCAL_INC_FOLDER)

#SOURCES=$(wildcard *.cc) $(wildcard *.c)
#OBJECTS=$(patsubst %.c, $(OBJDIR)/%.co, $(patsubst %.cc, $(OBJDIR)/%.cpo, $(SOURCES)))
CSOURCES=$(wildcard *.c)
CPPSOURCES=$(wildcard *.cc)
COBJECTS=$(patsubst %.c, $(OBJDIR)/%.co, $(CSOURCES))
CPPOBJECTS=$(patsubst %.cc, $(OBJDIR)/%.cpo, $(CPPSOURCES))
CPPFLAGS=$(CPPSTANDARD) $(WARNINGS) $(DEBUG) $(OPTIMISATIONS) $(CPPINC_FOLDER) $(CUSTOM_FLAGS)
CFLAGS = $(CSTANDARD) $(WARNINGS) $(DEBUG) $(OPTIMISATIONS) $(CINC_FOLDER) $(CUSTOM_FLAGS)

DEP=depend

DEPENDS:=$(patsubst %.c, %.dep, $(wildcard *.c)) $(patsubst %.cc, %.dep, $(wildcard *.cc))
CURR_DEP:=$(wildcard $(DEP))
CURR_DEPENDS:=$(wildcard $(DEPENDS))

BIN=systest

%.dep: %.c
	gcc $(CSTANDARD) -MM $< -MT $(patsubst %.c, $(OBJDIR)/%.co, $<) $(CINC_FOLDER) -o $@ 

%.dep: %.cc
	g++ $(CPPSTANDARD) -MM $< -MT $(patsubst %.cc, $(OBJDIR)/%.cpo, $<) $(CPPINC_FOLDER) -o $@ 

$(OBJDIR)/%.cpo: %.cc
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(OBJDIR)/%.co: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all
all: libgmock.a $(COBJECTS) $(CPPOBJECTS) $(DEP) $(BIN) genfile

.PHONY: clean
clean:
	rm -f obj/*.co obj/*.cpo $(BIN) $(CURR_DEPENDS) $(CURR_DEP) *.dep

systest: $(COBJECTS) ../lib/libcpiol.so
	$(CC) -o $@ $(CFLAGS) $(CLDFLAGS) $(COBJECTS)

genfile: $(CPPOBJECTS) ../lib/libpiol.so
	$(CPP) -o $@ $(CPPFLAGS) $(CPPLDFLAGS) $(CPPOBJECTS)

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

ifeq "$(CURR_DEP)" "$(DEP)"
-include $(DEP)
endif

$(DEP): $(DEPENDS)
	cat *.dep > $(DEP)

