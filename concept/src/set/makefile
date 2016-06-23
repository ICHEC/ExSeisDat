include ../compiler.cfg
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

test:
	echo $(DEP) $(OBJECTS) 

$(DEP): $(DEPENDS)
	cat *.dep > $(DEP)

.PHONY: clean
clean:
	rm -f $(CURR_OBJECTS) $(DEPENDS) $(CURR_DEP)

-include $(DEP)

