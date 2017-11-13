all:
	$(MAKE) -C src
	LIBRARY_PATH=$(LIBRARY_PATH):$(PWD)/lib $(MAKE) -C api
	LIBRARY_PATH=$(LIBRARY_PATH):$(PWD)/lib $(MAKE) -C test
	LIBRARY_PATH=$(LIBRARY_PATH):$(PWD)/lib $(MAKE) -C util
	LIBRARY_PATH=$(LIBRARY_PATH):$(PWD)/lib $(MAKE) -C examples

clean:
	$(MAKE) -C src  clean
	$(MAKE) -C test clean
	$(MAKE) -C api  clean
	$(MAKE) -C util clean
	$(MAKE) -C examples clean

