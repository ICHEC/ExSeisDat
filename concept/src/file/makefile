include ../compiler.cfg
DEP=#DEPEND
LDFLAGS = 
SOURCES=$(wildcard *.cc)
OBJECTS=$(patsubst %.cc, $(OBJDIR)/%.o, $(SOURCES))
DEPENDS=$(patsubst %.cc, %.dep, $(SOURCES))

CURR_OBJECTS=$(filter $(wildcard $(OBJDIR)/*.o), $(OBJECTS))
#CURR_DEP=$(wildcard $(DEP))

#$(BIN): $(OBJECTS)

#-include $(DEP)

all: $(DEP) $(OBJECTS)

#$(DEP):
#	gcc -std=c++14 -MM $< -MT  $(INC_FOLDER) -o $(DEP)

$(OBJDIR)/%.o: %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

%.dep: %.cc

clean:
	rm $(OBJECTS)
	#rm $(CURR_OBJECTS) $(CURR_DEP)


