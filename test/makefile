GTEST_DIR=../googletest/googletest
GMOCK_DIR=../googletest/googlemock
GOOGLE_OBJ=$(OBJDIR)/gmock-all.o  $(OBJDIR)/gtest-all.o
TEST_FRAM=-I../googletest/googletest/include/ -I../googletest/googlemock/include/
LOCAL_INC_FOLDER=-I../src/ $(TEST_FRAM)
OBJDIR=obj
BINDIR=bin
LIB_IPIOL=ipiol
LIB_PIOL=piol
GCOMP=$(CXX)

include ../compiler.cfg

DEP=depend
SPECTESTS_SOURCES=$(wildcard spectests/*.cc)
SPECTESTS_OBJECTS=$(patsubst spectests/%.cc, $(OBJDIR)/%_spectests.o, $(SPECTESTS_SOURCES))
SPECTESTS_DEPENDS=$(patsubst spectests/%.cc, $(OBJDIR)/%_spectests.dep, $(SPECTESTS_SOURCES))

CWRAPTESTS_SOURCES=$(wildcard cwraptests/*.cc)
CWRAPTESTS_OBJECTS=$(patsubst cwraptests/%.cc, $(OBJDIR)/%_cwraptests.o, $(CWRAPTESTS_SOURCES))
CWRAPTESTS_DEPENDS=$(patsubst cwraptests/%.cc, $(OBJDIR)/%_cwraptests.dep, $(CWRAPTESTS_SOURCES))


CURR_OBJECTS=$(filter $(wildcard $(OBJDIR)/*.o), $(SPECTESTS_OBJECTS))
CURR_DEP=$(wildcard $(DEP))
CURR_DEPENDS=$(wildcard $(SPECTESTS_DEPENDS))

BIN=$(BINDIR)/spectests

.PHONY: all
all: $(BINDIR)/extract $(BINDIR)/genfiles $(BINDIR)/libgmock.a $(BINDIR)/spectests $(BINDIR)/cwraptests $(DEP) $(SPECTESTS_OBJECTS)

$(LIB_DIR)/$(LIB_IPIOL):
	(cd ../src/; $(MAKE))

$(LIB_DIR)/$(LIB_PIOL):
	(cd ../api/; $(MAKE))

$(BINDIR)/libgmock.a: $(BINDIR)/.bindir_stamp $(OBJDIR)/.objdir_stamp
	$(GCOMP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
    -isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
    -pthread -c ${GTEST_DIR}/src/gtest-all.cc -o $(OBJDIR)/gtest-all.o
	$(GCOMP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
    -isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
    -pthread -c ${GMOCK_DIR}/src/gmock-all.cc -o $(OBJDIR)/gmock-all.o
	ar -rv $(BINDIR)/libgmock.a $(GOOGLE_OBJ)

$(BINDIR)/.bindir_stamp:
	mkdir -p $(BINDIR)
	touch $(BINDIR)/.bindir_stamp

$(OBJDIR)/.objdir_stamp:
	mkdir -p $(OBJDIR)
	touch $(OBJDIR)/.objdir_stamp

$(BINDIR)/spectests: $(BINDIR)/.bindir_stamp $(SPECTESTS_OBJECTS) $(BINDIR)/libgmock.a
	$(CXX) -o $@ -L$(LIB_DIR) $(CXXFLAGS) $(LOCAL_INC_FOLDER) $(CXXLDFLAGS) $(SPECTESTS_OBJECTS) $(BINDIR)/libgmock.a -l$(LIB_PIOL) -l$(LIB_IPIOL)

$(BINDIR)/cwraptests: $(BINDIR)/.bindir_stamp $(CWRAPTESTS_OBJECTS) $(BINDIR)/libgmock.a
	$(CXX) -o $@ -L$(LIB_DIR) $(CXXFLAGS) $(LOCAL_INC_FOLDER) $(CWRAPTESTS_OBJECTS) $(BINDIR)/libgmock.a ../api/obj/cfileapi.o 


$(BINDIR)/genfiles: genfiles.c $(BINDIR)/.bindir_stamp
	$(CC) -g -Wall -Wextra -std=c11 $< -o $@

$(BINDIR)/extract: extract.c   $(BINDIR)/.bindir_stamp
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
	$(BINDIR)/spectests --gtest_filter=-*Farm*
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
	rm -f $(BINDIR)/spectests $(BINDIR)/genfiles $(BINDIR)/extract $(BINDIR)/.bindir_stamp
	- rmdir $(BINDIR)
	rm -rf tmp/ *.dSYM $(BINDIR) $(OBJDIR)

.PHONY: googleclean
googleclean:
	rm -f $(BINDIR)/libgmock.a $(GOOGLE_OBJ)

$(DEP): $(SPECTESTS_DEPENDS)
	cat $(OBJDIR)/*.dep > $(DEP)

ifeq "$(CURR_DEP)" "$(DEP)"
-include $(DEP)
endif

%.dep: %.cc
	$(CXX) $(CXXSTANDARD) $(CXXINC_FOLDER) $(LOCAL_INC_FOLDER) -MM $< -MT $(patsubst %.cc, $(OBJDIR)/%.o, $<) -o $@

$(OBJDIR)/%.o: %.cc $(OBJDIR)/.objdir_stamp
	$(CXX) $(CXXFLAGS) $(LOCAL_INC_FOLDER) -c $< -o $@

$(OBJDIR)/%_spectests.dep: spectests/%.cc $(OBJDIR)/.objdir_stamp
	$(CXX) $(CXXSTANDARD) $(CXXINC_FOLDER) $(LOCAL_INC_FOLDER) -MM $< -MT $(patsubst %.cc, $(OBJDIR)/%.o, $<) -o $@

$(OBJDIR)/%_spectests.o: spectests/%.cc $(OBJDIR)/.objdir_stamp
	$(CXX) $(CXXFLAGS) $(LOCAL_INC_FOLDER) -c $< -o $@

$(OBJDIR)/%_cwraptests.dep: cwraptests/%.cc $(OBJDIR)/.objdir_stamp
	$(CXX) $(CXXSTANDARD) $(CXXINC_FOLDER) $(LOCAL_INC_FOLDER) -MM $< -MT $(patsubst %.cc, $(OBJDIR)/%.o, $<) -o $@

$(OBJDIR)/%_cwraptests.o: cwraptests/%.cc $(OBJDIR)/.objdir_stamp
	$(CXX) $(CXXFLAGS) $(LOCAL_INC_FOLDER) -c $< -o $@

