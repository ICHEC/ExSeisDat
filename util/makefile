OBJDIR=obj

GCOMP=g++

#IMELD_FLAG=-Wl,--dynamic-linker=/ichec/home/staff/cathal/env/extract/lib64/ld-linux-x86-64.so.2 
include ../compiler.cfg

DEP=depend
DEPENDS:=$(patsubst %.c, %.dep, $(wildcard *.c)) $(patsubst %.cc, %.dep, $(wildcard *.cc))
CURR_DEP:=$(wildcard $(DEP))
CURR_DEPENDS:=$(wildcard $(DEPENDS))
BIN = fourdbin cropen cexample1 cexample2 creadwrite assess example1 filemake makerep makerepn1 makerepn2 minmax
CURR_BIN=$(wildcard $(BIN))

%.dep: %.c
	gcc $(CSTANDARD) -MM $< -MT $(patsubst %.c, $(OBJDIR)/%.co, $<) $(CINC_FOLDER) -o $@ 

%.dep: %.cc
	g++ $(CXXSTANDARD) -MM $< -MT $(patsubst %.cc, $(OBJDIR)/%.cpo, $<) $(CXXINC_FOLDER) -o $@ 

$(OBJDIR)/%.cpo: %.cc
	$(CXX) $(CXXFLAGS) $(CXXINC_FOLDER) -c $< -o $@

$(OBJDIR)/%.co: %.c
	$(CC) $(CFLAGS) $(CINC_FOLDER) -c $< -o $@

.PHONY: all
all: $(BIN)
# 

.PHONY: clean
clean:
	rm -f obj/*.co obj/*.cpo $(CURR_DEPENDS) $(CURR_DEP) *.dep $(CURR_BIN)

cropen: obj/cropen.co obj/sglobal.co obj/ctest.co
	$(CC) $(CFLAGS) $(CLDFLAGS) $+ -o $@ 
cexample1: obj/example1.co obj/sglobal.co
	$(CC) $(CFLAGS) $(CLDFLAGS) $+ -o $@ 
cexample2: obj/example2.co obj/sglobal.co
	$(CC) $(CFLAGS) $(CLDFLAGS) $+ -o $@ 
creadwrite: obj/creadwrite.co obj/sglobal.co
	$(CC) $(CFLAGS) $(CLDFLAGS) $+ -o $@ 
assess: obj/assess.cpo obj/sglobal.cpo
	$(CXX) $(CXXFLAGS) $(CXXLDFLAGS) $+ -o $@ 
example1: obj/example1.cpo obj/sglobal.cpo
	$(CXX) $(CXXFLAGS) $(CXXLDFLAGS) $+ -o $@ 
segsort: obj/sort.cpo obj/sglobal.cpo
	$(CXX) $(CXXFLAGS) $(CXXLDFLAGS) $+ -o $@
fourdbin: obj/fourdbin.cpo obj/sglobal.cpo
	$(CXX) $(CXXFLAGS) $(CXXLDFLAGS) $+ -o $@ 
filemake: obj/filemake.cpo obj/sglobal.cpo
	$(CXX) $(IMELD_FLAG) $(CXXFLAGS) $(CXXLDFLAGS) $+ -o $@ 
makerep: obj/makerep.cpo obj/sglobal.cpo
	$(CXX) $(IMELD_FLAG) $(CXXFLAGS) $(CXXLDFLAGS) $+ -o $@ 
makerepn1: obj/makerep.cpo obj/sglobal.cpo
	$(CXX) $(CXXFLAGS) $(CXXLDFLAGS) $+ -o $@ 
makerepn2: obj/makerep.cpo obj/sglobal.cpo
	$(CXX) $(CXXFLAGS) $(CXXLDFLAGS) $+ -o $@ 
minmax: obj/minmax.cpo obj/sglobal.cpo
	$(CXX) $(CXXFLAGS) $(CXXLDFLAGS) $+ -o $@ 

ifeq "$(CURR_DEP)" "$(DEP)"
-include $(DEP)
endif

$(DEP): $(DEPENDS)
	cat *.dep > $(DEP)

