all:
	(cd test; $(MAKE) libgmock.a)
	(cd src; $(MAKE))
	(cd test; $(MAKE))
	(cd unittest; $(MAKE))

clean:
	(cd src; $(MAKE) clean)
	(cd test; $(MAKE) clean)
	(cd unittest; $(MAKE) clean)
