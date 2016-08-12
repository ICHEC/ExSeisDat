INC_FOLDER = -I../include
CUSTOM_FLAGS=-DNT_LIMITS=10000000

include ../compiler.cfg
BIN=$(LIB_DIR)/libpiol.so
CPPFLAGS = $(CPPSTANDARD) $(WARNINGS) $(DEBUG) $(OPTIMISATIONS) $(INC_FOLDER) $(CUSTOM_FLAGS)
OBJDIR = obj

DEP=depend
SOURCES:=$(wildcard *.cc)
OBJECTS:=$(patsubst %.cc, $(OBJDIR)/%.o, $(SOURCES))
DEPENDS:=$(patsubst %.cc, %.dep, $(SOURCES))

CURR_OBJECTS:=$(filter $(wildcard $(OBJDIR)/*.o), $(OBJECTS))
CURR_DEP:=$(wildcard $(DEP))

%.dep: %.cc
	g++ $(CPPSTANDARD) -MM $< -MT $(patsubst %.cc, $(OBJDIR)/%.o, $<) $(INC_FOLDER) -o $@ 

$(OBJDIR)/%.o: %.cc
	$(CPP) $(LIB_FLAG) $(CPPFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: all
all: $(DEP) $(OBJECTS) $(BIN)

$(BIN): $(OBJECTS)
	$(CPP) -o $@ -shared $(CPPFLAGS) $(LDFLAGS) $(OBJECTS)

$(DEP): $(DEPENDS)
	cat *.dep > $(DEP)

ifeq "$(CURR_DEP)" "$(DEP)"
-include $(DEP)
endif

.PHONY: clean
clean:
	rm -f $(CURR_OBJECTS) *.dep $(CURR_DEP)

