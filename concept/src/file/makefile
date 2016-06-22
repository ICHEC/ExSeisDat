include ../compiler.cfg
LDFLAGS = 
SOURCES=$(wildcard *.cc)
#OBJECTS=$(SOURCES:.cc=.o)
#OBJECTS=$(SOURCES:.cc=.o)
OBJECTS=$(patsubst %.cc, $(OBJDIR)/%.o, $(SOURCES))

#$(BIN): $(OBJECTS)
all: $(OBJECTS)

$(OBJDIR)/%.o: %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean :
	rm $(OBJECTS)
