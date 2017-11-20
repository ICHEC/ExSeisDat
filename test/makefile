GTEST_DIR=../googletest/googletest
GMOCK_DIR=../googletest/googlemock
GOOGLE_OBJ=$(OBJDIR)/gmock-all.o  $(OBJDIR)/gtest-all.o
TEST_FRAM=-I../googletest/googletest/include/ -I../googletest/googlemock/include/
LOCAL_INC_FOLDER=-I../src/ $(TEST_FRAM) -I.
OBJDIR=obj
BINDIR=bin
LIB_IPIOL=ipiol
LIB_PIOL=piol
GCOMP=$(CXX)

include ../compiler.cfg

DEP=depend
SPECTESTS_SOURCES=$(wildcard spectests/*.cc)
SPECTESTS_OBJECTS=$(patsubst spectests/%.cc, $(OBJDIR)/spectests/%.o,   $(SPECTESTS_SOURCES))
SPECTESTS_DEPENDS=$(patsubst spectests/%.cc, $(OBJDIR)/spectests/%.dep, $(SPECTESTS_SOURCES))

PIOLSTUB_SOURCES=$(wildcard piolstub/*.cc)
PIOLSTUB_OBJECTS=$(patsubst piolstub/%.cc, $(OBJDIR)/piolstub/%.o,   $(PIOLSTUB_SOURCES))
PIOLSTUB_DEPENDS=$(patsubst piolstub/%.cc, $(OBJDIR)/piolstub/%.dep, $(PIOLSTUB_SOURCES))

CWRAPTESTS_SOURCES=$(wildcard cwraptests/*.cc)
CWRAPTESTS_OBJECTS=$(patsubst cwraptests/%.cc, $(OBJDIR)/cwraptests/%.o,   $(CWRAPTESTS_SOURCES))
CWRAPTESTS_DEPENDS=$(patsubst cwraptests/%.cc, $(OBJDIR)/cwraptests/%.dep, $(CWRAPTESTS_SOURCES))

WRAPTESTS_SOURCES=$(wildcard wraptests/*.cc)
WRAPTESTS_OBJECTS=$(patsubst wraptests/%.cc, $(OBJDIR)/wraptests/%.o,   $(WRAPTESTS_SOURCES))
WRAPTESTS_DEPENDS=$(patsubst wraptests/%.cc, $(OBJDIR)/wraptests/%.dep, $(WRAPTESTS_SOURCES))


CURR_OBJECTS=$(SPECTESTS_OBJECTS) $(PIOLSTUB_OBJECTS) $(WRAPTESTS_OBJECTS) $(CWRAPTESTS_OBJECTS)
CURR_DEP=$(wildcard $(DEP))
CURR_DEPENDS=$(SPECTESTS_DEPENDS) $(PIOLSTUB_DEPENDS) $(WRAPTESTS_DEPENDS) $(CWRAPTESTS_DEPENDS)

BIN=$(BINDIR)/spectests

.PHONY: all
all: $(BINDIR)/extract $(BINDIR)/genfiles $(BINDIR)/spectests $(BINDIR)/cwraptests $(DEP) $(SPECTESTS_OBJECTS)

$(LIB_DIR)/$(LIB_IPIOL):
	(cd ../src/; $(MAKE))

$(LIB_DIR)/$(LIB_PIOL):
	(cd ../api/; $(MAKE))

$(BINDIR)/libgmock.a:
	mkdir -p $(@D)
	mkdir -p $(OBJDIR)
	$(GCOMP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
	-isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
	-pthread -c ${GTEST_DIR}/src/gtest-all.cc -o $(OBJDIR)/gtest-all.o
	$(GCOMP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
	-isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
	-pthread -c ${GMOCK_DIR}/src/gmock-all.cc -o $(OBJDIR)/gmock-all.o
	ar -rv $(BINDIR)/libgmock.a $(GOOGLE_OBJ)

$(BINDIR)/spectests: $(SPECTESTS_OBJECTS) $(BINDIR)/libgmock.a
	mkdir -p $(@D)
	$(CXX) -o $@ -L$(LIB_DIR) $(CXXFLAGS) $(LOCAL_INC_FOLDER) $(CXXLDFLAGS) $(SPECTESTS_OBJECTS) $(BINDIR)/libgmock.a -l$(LIB_PIOL) -l$(LIB_IPIOL)

$(BINDIR)/cwraptests: $(CWRAPTESTS_OBJECTS) ../api/obj/cfileapi.o $(BINDIR)/wraptests.so $(BINDIR)/piolstub.so
	$(CXX) -o $@ -L$(LIB_DIR) $(CXXFLAGS) $(LOCAL_INC_FOLDER) $^  -Wl,-dead_strip

$(BINDIR)/wraptests.so: $(WRAPTESTS_OBJECTS) $(BINDIR)/piolstub.so $(BINDIR)/libgmock.a
	$(CXX) -shared -o $@ -L$(LIB_DIR) $(CXXFLAGS) $(LOCAL_INC_FOLDER) $^

$(BINDIR)/piolstub.so: $(PIOLSTUB_OBJECTS)
	$(CXX) -shared -o $@ -L$(LIB_DIR) $(CXXFLAGS) $(LOCAL_INC_FOLDER) $^


$(BINDIR)/genfiles: genfiles.c
	mkdir -p $(@D)
	$(CC) -g -Wall -Wextra -std=c11 $< -o $@

$(BINDIR)/extract: extract.c
	mkdir -p $(@D)
	$(CC) -g -Wall -Wextra -std=c11 $< -o $@


# Testing commands
# On a regular computer, use `make test_nofarm` because farm tests
# use large files, so need a lot of memory.

# Run all tests
.PHONY: test
test: $(BINDIR)/spectests tmp/.genfiles_stamp
	$(BINDIR)/spectests

# Run non-farm tests
.PHONY: test_nofarm
test_nofarm: $(BINDIR)/spectests $(BINDIR)/cwraptests tmp/.genfiles_stamp
	#$(BINDIR)/spectests --gtest_filter=-*Farm*
	$(BINDIR)/cwraptests

# Generate all test files
tmp/.genfiles_stamp: $(BINDIR)/genfiles
	mkdir -p tmp
	$(BINDIR)/genfiles
	touch tmp/.genfiles_stamp

# Clean everything
.PHONY: clean
clean: googleclean
	rm -f $(SPECTESTS_OBJECTS) $(OBJDIR)/*.optrpt $(CURR_DEPENDS) $(CURR_DEP)
	rm -f $(BINDIR)/spectests $(BINDIR)/genfiles $(BINDIR)/extract
	- rmdir $(BINDIR)
	rm -rf tmp/ *.dSYM $(BINDIR) $(OBJDIR)

.PHONY: googleclean
googleclean:
	rm -f $(BINDIR)/libgmock.a $(GOOGLE_OBJ)

$(DEP): $(CURR_DEPENDS)
	cat $< > $(DEP)

ifeq "$(CURR_DEP)" "$(DEP)"
-include $(DEP)
endif

$(OBJDIR)/%.dep: %.cc
	mkdir -p $(@D)
	$(CXX) $(CXXSTANDARD) $(CXXINC_FOLDER) $(LOCAL_INC_FOLDER) -MM $< -MT $(patsubst %.cc, $(OBJDIR)/%.o, $<) -o $@

$(OBJDIR)/%.o: %.cc
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(LOCAL_INC_FOLDER) -c $< -o $@
