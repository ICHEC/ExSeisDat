OBJDIR=obj

#IMELD_FLAG=-Wl,--dynamic-linker=/ichec/home/staff/cathal/env/extract/lib64/ld-linux-x86-64.so.2 
include ../compiler.cfg

DEP=depend
DEPENDS:=$(patsubst %.c, %.dep, $(wildcard *.c)) $(patsubst %.cc, %.dep, $(wildcard *.cc))
CURR_DEP:=$(wildcard $(DEP))
CURR_DEPENDS:=$(wildcard $(DEPENDS))
BIN = concatenate segsort versort fourdbin cropen creadwrite assess filemake makerep makerepn1 makerepn2 minmax traceanalysis
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

.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.co $(OBJDIR)/*.cpo $(OBJDIR)/*.optrpt $(CURR_DEPENDS) $(CURR_DEP) *.dep $(CURR_BIN)

cropen: obj/cropen.co obj/sglobal.co obj/ctest.co
	$(CC) $(CFLAGS) $+ -o $@ $(CLDFLAGS)
traceanalysis: obj/traceanalysis.cpo
	$(CXX) $(CXXFLAGS) $+ -o $@ $(CXXLDFLAGS)
assess: obj/assess.cpo obj/sglobal.cpo
	$(CXX) $(CXXFLAGS) $+ -o $@ $(CXXLDFLAGS)
versort: obj/verifysort.cpo obj/sglobal.cpo
	$(CXX) $(CXXFLAGS) $+ -o $@ $(CXXLDFLAGS)
concatenate: obj/concatenate.cpo obj/sglobal.cpo
	$(CXX) $(CXXFLAGS) $+ -o $@ $(CXXLDFLAGS)
creadwrite: obj/concatenate.cpo obj/sglobal.cpo
	$(CXX) $(CXXFLAGS) $+ -o $@ $(CXXLDFLAGS)
segsort: obj/sort.cpo
	$(CXX) $(CXXFLAGS) $+ -o $@ $(CXXLDFLAGS)
fourdbin: obj/fourdbin.cpo obj/4dio.cpo obj/4dcore.cpo obj/sglobal.cpo
	$(CXX) $(CXXFLAGS) $+ -o $@ $(CXXLDFLAGS)
fourdbinRestart: obj/fourdbinRestart.cpo obj/4dio.cpo obj/4dcore.cpo obj/sglobal.cpo
	$(CXX) $(CXXFLAGS) $+ -o $@ $(CXXLDFLAGS)
filemake: obj/filemake.cpo obj/sglobal.cpo
	$(CXX) $(IMELD_FLAG) $(CXXFLAGS) $+ -o $@ $(CXXLDFLAGS)
makerep: obj/makerep.cpo obj/sglobal.cpo
	$(CXX) $(IMELD_FLAG) $(CXXFLAGS) $+ -o $@ $(CXXLDFLAGS)
makerepn1: obj/makerep.cpo obj/sglobal.cpo
	$(CXX) $(CXXFLAGS) $+ -o $@ $(CXXLDFLAGS)
makerepn2: obj/makerep.cpo obj/sglobal.cpo
	$(CXX) $(CXXFLAGS) $+ -o $@ $(CXXLDFLAGS)
minmax: obj/minmax.cpo obj/sglobal.cpo
	$(CXX) $(CXXFLAGS) $+ -o $@ $(CXXLDFLAGS)

ifeq "$(CURR_DEP)" "$(DEP)"
-include $(DEP)
endif

$(DEP): $(DEPENDS)
	cat *.dep > $(DEP)

