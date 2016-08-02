# Basic settings
# Have the following defined before using this:
# LOCAL_INC_FOLDER

INC_FOLDER = -I../ $(LOCAL_INC_FOLDER)
CUSTOM_FLAGS=-DNT_LIMITS=10000000
CXXFLAGS = $(STANDARDS) $(WARNINGS) $(DEBUG) $(OPTIMISATIONS) $(INC_FOLDER) $(CUSTOM_FLAGS)

include ../../compiler.cfg

ifndef OBJDIR
OBJDIR = ../obj
endif


DEP=depend
SOURCES:=$(wildcard *.cc)
OBJECTS:=$(patsubst %.cc, $(OBJDIR)/%.o, $(SOURCES))
DEPENDS:=$(patsubst %.cc, %.dep, $(SOURCES))
CURR_OBJECTS:=$(filter $(wildcard $(OBJDIR)/*.o), $(OBJECTS))
CURR_DEP:=$(wildcard $(DEP))

%.dep: %.cc
	gcc -std=c++14 -MM $< -MT $(patsubst %.cc, $(OBJDIR)/%.o, $<) $(INC_FOLDER) -o $@ 

$(OBJDIR)/%.o: %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: all
all: $(OBJECTS) $(DEP) 

$(DEP): $(DEPENDS)
	cat *.dep > $(DEP)

.PHONY: clean
clean:
	rm -f $(CURR_OBJECTS) *.dep $(CURR_DEP)

ifeq "$(CURR_DEP)" "$(DEP)"
-include $(DEP)
endif
