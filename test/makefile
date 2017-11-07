GTEST_DIR=../googletest/googletest
GMOCK_DIR=../googletest/googlemock
GOOGLE_OBJ=gmock-all.o  gtest-all.o
TEST_FRAM=-I../googletest/googletest/include/ -I../googletest/googlemock/include/
LOCAL_INC_FOLDER=-I../src/ $(TEST_FRAM)
OBJDIR=obj
LIB_IPIOL=ipiol
LIB_PIOL=piol
GCOMP=$(CXX)

include ../compiler.cfg

DEP=depend
SOURCES=$(wildcard *.cc)
OBJECTS=$(patsubst %.cc, $(OBJDIR)/%.o, $(SOURCES))
DEPENDS=$(patsubst %.cc, %.dep, $(SOURCES))

CURR_OBJECTS=$(filter $(wildcard $(OBJDIR)/*.o), $(OBJECTS))
CURR_DEP=$(wildcard $(DEP))
CURR_DEPENDS=$(wildcard $(DEPENDS))

BIN=spectests

.PHONY: all
all: extract genfiles libgmock.a $(BIN) $(DEP) $(OBJECTS)

$(LIB_DIR)/$(LIB_IPIOL):
	(cd ../src/; $(MAKE))

$(LIB_DIR)/$(LIB_PIOL):
	(cd ../api/; $(MAKE))

libgmock.a:
	$(GCOMP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
    -isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
    -pthread -c ${GTEST_DIR}/src/gtest-all.cc
	$(GCOMP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
    -isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
    -pthread -c ${GMOCK_DIR}/src/gmock-all.cc
	ar -rv libgmock.a gtest-all.o gmock-all.o

$(BIN): $(OBJECTS)
	$(CXX) -o $@ -L$(LIB_DIR) $(CXXFLAGS) $(LOCAL_INC_FOLDER) $(CXXLDFLAGS) $(OBJECTS) $(GOOGLE_OBJ) -l$(LIB_PIOL) -l$(LIB_IPIOL)

genfiles: genfiles.c
	$(CC) -g -Wall -Wextra -std=c11 genfiles.c -o genfiles

extract: extract.c
	$(CC) -g -Wall -Wextra -std=c11 extract.c -o extract


# Testing commands
# First time use `make testfiles` before `make test`.
# On a regular computer, use `make test_nofarm` because farm tests
# use large files, so need a lot of memory.

# Run all tests
.PHONY: test
test: spectests tmp/.genfiles_stamp
	./spectests

# Run non-farm tests
.PHONY: test_nofarm
test_nofarm: spectests tmp/.genfiles_stamp
	#./spectests --gtest_filter=-*Farm*
	./spectests --gtest_filter='*FileSEGYIntegRead*'

# Generate all test files
tmp/.genfiles_stamp: genfiles
	mkdir -p tmp
	./genfiles
	touch tmp/.genfiles_stamp

# Clean everything
.PHONY: clean
clean:
	rm -f $(OBJECTS) $(OBJDIR)/*.optrpt $(BIN) $(CURR_DEPENDS) $(CURR_DEP)
	rm -rf tmp/ *.dSYM

googleclean:
	rm -f libgmock.a gtest-all.o gmock-all.o

$(DEP): $(DEPENDS)
	cat *.dep > $(DEP)

ifeq "$(CURR_DEP)" "$(DEP)"
-include $(DEP)
endif

%.dep: %.cc
	$(CXX) $(CXXSTANDARD) $(CXXINC_FOLDER) $(LOCAL_INC_FOLDER) -MM $< -MT $(patsubst %.cc, $(OBJDIR)/%.o, $<) -o $@

$(OBJDIR)/%.o: %.cc
	$(CXX) $(CXXFLAGS) $(LOCAL_INC_FOLDER) -c $< -o $@

