
include ../compiler.cfg
BIN=$(LIB_DIR)/libipiol.so
OBJDIR = obj
DEP=depend
SOURCES:=$(wildcard *.cc)
OBJECTS:=$(patsubst %.cc, $(OBJDIR)/%.o, $(SOURCES))
DEPENDS:=$(patsubst %.cc, %.dep, $(SOURCES))

CURR_OBJECTS:=$(filter $(wildcard $(OBJDIR)/*.o), $(OBJECTS))
CURR_DEP:=$(wildcard $(DEP))

%.dep: %.cc
	$(CXX) $(CXXSTANDARD) -MM $< -MT $(patsubst %.cc, $(OBJDIR)/%.o, $<) $(CXXINC_FOLDER) -o $@

$(OBJDIR)/%.o: %.cc
	$(CXX) -fPIC $(CXXFLAGS) -c $< -o $@

.PHONY: all
all: $(DEP) $(OBJECTS) $(BIN)

$(BIN): $(OBJECTS)
	$(CXX) $(LIB_FLAG) $(IMELD_FLAG) -shared $(CXXFLAGS) -o $@ $(OBJECTS)

$(DEP): $(DEPENDS)
	cat *.dep > $(DEP)

ifeq "$(CURR_DEP)" "$(DEP)"
-include $(DEP)
endif

.PHONY: clean
clean:
	rm -f $(CURR_OBJECTS) $(OBJDIR)/*.optrpt *.dep $(CURR_DEP) $(BIN)

