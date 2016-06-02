SOURCES=$(wildcard *.dot)
OBJECTS=$(SOURCES:.dot=.svg)

all: $(OBJECTS)

$(OBJECTS): %.svg: %.dot
	dot -T svg -o $@ $<

clean:
	rm *.svg

