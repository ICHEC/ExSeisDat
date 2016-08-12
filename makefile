all:
	(cd test; $(MAKE) libgmock.a)
	(cd src; $(MAKE))
	(cd test; $(MAKE))
	(cd unittest; $(MAKE))
	(cd api; $(MAKE))
	(cd sys; $(MAKE))

clean:
	(cd src; $(MAKE) clean)
	(cd test; $(MAKE) clean)
	(cd unittest; $(MAKE) clean)
	(cd api; $(MAKE) clean)
	(cd sys; $(MAKE) clean)

