INC_FOLDER = -I../include

include ../compiler.cfg

CXXFLAGS = $(CXXSTANDARD) $(WARNINGS) $(DEBUG) $(OPTIMISATIONS) $(INC_FOLDER) $(CUSTOM_FLAGS)
OBJDIR = obj

CXXBIN=piol
BIN=$(LIB_DIR)/libcpiol.so

DEP=depend
SOURCES:=$(wildcard *.cc)
OBJECTS:=$(patsubst %.cc, $(OBJDIR)/%.o, $(SOURCES))
OBJECTS=$(patsubst %.cc, $(OBJDIR)/%.o, $(SOURCES))

DEPENDS:=$(patsubst %.cc, %.dep, $(SOURCES))

CURR_OBJECTS:=$(filter $(wildcard $(OBJDIR)/*.o), $(OBJECTS))
CURR_DEP:=$(wildcard $(DEP))

%.dep: %.cc
	g++ $(CXXSTANDARD) -MM $< -MT $(patsubst %.cc, $(OBJDIR)/%.o, $<) $(INC_FOLDER) -o $@ 

$(OBJDIR)/%.o: %.cc
	$(CXX) $(LIB_FLAG) $(CXXFLAGS) -c $< -o $@

.PHONY: all
all: $(DEP) $(OBJECTS) $(BIN)

$(LIB_DIR)/lib$(CXXBIN).so:
	cd ../src ; $(MAKE)

$(BIN): $(OBJECTS) $(LIB_DIR)/lib$(CXXBIN).so
	$(CXX) -o $@ -shared $(LIB_FLAG) $(CXXFLAGS) -L$(LIB_DIR) -l$(CXXBIN) $(LDFLAGS) $(OBJECTS)

$(DEP): $(DEPENDS)
	cat *.dep > $(DEP)

ifeq "$(CURR_DEP)" "$(DEP)"
-include $(DEP)
endif

.PHONY: clean
clean:
	rm -f $(CURR_OBJECTS) *.dep $(CURR_DEP) $(BIN)

