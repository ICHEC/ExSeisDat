LDFLAGS=gmock-all.o  gtest-all.o $(TULLOW_LIBS)
TEST_FRAM=-I../googletest/googletest/include/ -I../googletest/googlemock/include/

include ../compiler.cfg

.PHONY: all
all: libgmock.a test

libgmock.a:
	$(GCOMP) -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
    -isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
    -pthread -c ${GTEST_DIR}/src/gtest-all.cc
	$(GCOMP) -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
    -isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
    -pthread -c ${GMOCK_DIR}/src/gmock-all.cc
	ar -rv libgmock.a gtest-all.o gmock-all.o

test: tests.o main.o 
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $+

.PHONY: clean
clean:
	rm -f test tests.o main.o *.optrpt

googleclean:
	rm -f libgmock.a gtest-all.o gmock-all.o

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(TEST_FRAM) -c $< -o $@

