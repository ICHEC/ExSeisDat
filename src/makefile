
include ../compiler.cfg
BIN=$(LIB_DIR)/libipiol.so
CXXFLAGS = $(CXXSTANDARD) $(WARNINGS) $(DEBUG) $(OPTIMISATIONS) $(CXXINC_FOLDER) $(CUSTOM_FLAGS)
OBJDIR = obj

DEP=depend
SOURCES:=$(wildcard *.cc)
OBJECTS:=$(patsubst %.cc, $(OBJDIR)/%.o, $(SOURCES))
DEPENDS:=$(patsubst %.cc, %.dep, $(SOURCES))

CURR_OBJECTS:=$(filter $(wildcard $(OBJDIR)/*.o), $(OBJECTS))
CURR_DEP:=$(wildcard $(DEP))

%.dep: %.cc
	g++ $(CXXSTANDARD) -MM $< -MT $(patsubst %.cc, $(OBJDIR)/%.o, $<) $(CXXINC_FOLDER) -o $@ 

$(OBJDIR)/%.o: %.cc
	$(CXX) $(LIB_FLAG) $(CXXFLAGS) -c $< -o $@

.PHONY: all
all: $(DEP) $(OBJECTS) $(BIN)

$(BIN): $(OBJECTS)
	$(CXX) -o $@ $(IMELD_FLAG) -shared $(CXXFLAGS) $(LDFLAGS) $(OBJECTS)

$(DEP): $(DEPENDS)
	cat *.dep > $(DEP)

ifeq "$(CURR_DEP)" "$(DEP)"
-include $(DEP)
endif

.PHONY: clean
clean:
	rm -f $(CURR_OBJECTS) *.dep $(CURR_DEP) $(BIN)

