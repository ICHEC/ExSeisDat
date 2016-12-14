OBJDIR=obj
#IMELD_FLAG=-Wl,--dynamic-linker=/ichec/home/staff/cathal/env/extract/lib64/ld-linux-x86-64.so.2 
include ../compiler.cfg

DEP=depend
DEPENDS:=$(patsubst %.c, %.dep, $(wildcard *.c)) $(patsubst %.cc, %.dep, $(wildcard *.cc))
CURR_DEP:=$(wildcard $(DEP))
CURR_DEPENDS:=$(wildcard $(DEPENDS))
BIN = sort minmax cmodprm csort cminmax hellow example1 cexample1 cexample2
CURR_BIN=$(wildcard $(BIN))
UTIL_INC=-I../util

%.dep: %.c
	gcc $(CSTANDARD) -MM $< -MT $(patsubst %.c, $(OBJDIR)/%.co, $<) $(UTIL_INC) $(CINC_FOLDER) -o $@ 

%.dep: %.cc
	g++ $(CXXSTANDARD) -MM $< -MT $(patsubst %.cc, $(OBJDIR)/%.cpo, $<) $(UTIL_INC) $(CXXINC_FOLDER) -o $@ 

$(OBJDIR)/%.cpo: %.cc
	$(CXX) $(CXXFLAGS) $(CXXINC_FOLDER) $(UTIL_INC) -c $< -o $@

$(OBJDIR)/%.co: %.c
	$(CC) $(CFLAGS) $(CINC_FOLDER) $(UTIL_INC) -c $< -o $@

.PHONY: all
all: $(BIN)

.PHONY: clean
clean:
	rm -f obj/*.co obj/*.cpo $(CURR_DEPENDS) $(CURR_DEP) *.dep $(CURR_BIN)

csort: obj/sort.co
	$(CC) $(CFLAGS) $(CLDFLAGS) $+ -o $@ 
minmax: obj/minmax.cpo
	$(CXX) $(CXXFLAGS) $(CXXLDFLAGS) $+ -o $@
sort: obj/sort.cpo
	$(CXX) $(CXXFLAGS) $(CXXLDFLAGS) $+ -o $@
cmodprm: obj/modprm.co ../util/obj/sglobal.co
	$(CC) $(CFLAGS) $(CLDFLAGS) $+ -o $@ 
hellow: obj/hello.co
	$(CC) $(CFLAGS) $(CLDFLAGS) $+ -o $@ 
cminmax: obj/minmax.co ../util/obj/sglobal.co
	$(CC) $(CFLAGS) $(CLDFLAGS) $+ -o $@ 
cexample1: obj/example1.co ../util/obj/sglobal.co
	$(CC) $(CFLAGS) $(CLDFLAGS) $+ -o $@ 
cexample2: obj/example2.co ../util/obj/sglobal.co
	$(CC) $(CFLAGS) $(CLDFLAGS) $+ -o $@ 
example1: obj/example1.cpo ../util/obj/sglobal.cpo
	$(CXX) $(CXXFLAGS) $(CXXLDFLAGS) $+ -o $@ 

ifeq "$(CURR_DEP)" "$(DEP)"
-include $(DEP)
endif

$(DEP): $(DEPENDS)
	cat *.dep > $(DEP)

