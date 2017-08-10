include ../compiler.cfg

#CXXFLAGS = $(CXXSTANDARD) $(WARNINGS) $(DEBUG) $(OPTIMISATIONS) $(INC_FOLDER) $(CUSTOM_FLAGS)
OBJDIR = obj

PIOLBIN=ipiol
CBIN=$(LIB_DIR)/libpiol.so

DEP=depend
SOURCES:=$(wildcard *.cc)
OBJECTS:=$(patsubst %.cc, $(OBJDIR)/%.o, $(SOURCES))
OBJECTS=$(patsubst %.cc, $(OBJDIR)/%.o, $(SOURCES))

DEPENDS:=$(patsubst %.cc, %.dep, $(SOURCES))

CURR_OBJECTS:=$(filter $(wildcard $(OBJDIR)/*.o), $(OBJECTS))
CURR_DEP:=$(wildcard $(DEP))

%.dep: %.cc
	g++ $(CXXSTANDARD) -MM $< -MT $(patsubst %.cc, $(OBJDIR)/%.o, $<) $(CXXINC_FOLDER) -o $@

$(OBJDIR)/%.o: %.cc
	$(CXX) -fPIC $(LIB_FLAG) $(CXXFLAGS) -c $< -o $@

.PHONY: all
all: $(DEP) $(OBJECTS) $(CBIN)

$(LIB_DIR)/lib$(PIOLBIN).so:
	cd ../src ; $(MAKE)

$(CBIN): $(OBJECTS) $(LIB_DIR)/lib$(PIOLBIN).so
	$(CXX) -o $@ -shared $(LIB_FLAG) $(CXXFLAGS) -L$(LIB_DIR) -l$(PIOLBIN) $(OBJECTS)

$(DEP): $(DEPENDS)
	cat *.dep > $(DEP)

ifeq "$(CURR_DEP)" "$(DEP)"
-include $(DEP)
endif

.PHONY: clean
clean:
	rm -f $(CURR_OBJECTS) $(OBJDIR)/*.optrpt *.dep $(CURR_DEP) $(CBIN)
