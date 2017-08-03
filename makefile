all:
	@(cd test; $(MAKE) libgmock.a)
	@(cd src; $(MAKE))
	@(cd api; LIBRARY_PATH=$(LIBRARY_PATH):$(PWD)/lib $(MAKE))
	@(cd test; LIBRARY_PATH=$(LIBRARY_PATH):$(PWD)/lib $(MAKE))
	@(cd util; LIBRARY_PATH=$(LIBRARY_PATH):$(PWD)/lib $(MAKE))
	@(cd examples; LIBRARY_PATH=$(LIBRARY_PATH):$(PWD)/lib $(MAKE))

clean:
	(cd src; $(MAKE) clean)
	(cd test; $(MAKE) clean)
	(cd api; $(MAKE) clean)
	(cd util; $(MAKE) clean)
	(cd examples; $(MAKE) clean)

