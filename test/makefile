
all:
	($(MAKE) -f makeunittest libgmock.a)
	(cd ../src; $(MAKE))
#	(cd anc; $(MAKE))
	(cd data; $(MAKE))
	(cd object; $(MAKE))
	(cd file; $(MAKE))
#	(cd share; $(MAKE))
	(cd main; $(MAKE))
	($(MAKE) -f makeunittest)

clean:
	(cd ../src; $(MAKE) clean)
#	(cd anc; $(MAKE) clean)
	(cd data; $(MAKE) clean)
	(cd object; $(MAKE) clean)
	(cd file; $(MAKE) clean)
#	(cd share; $(MAKE))
	(cd main; $(MAKE) clean)
	($(MAKE) -f makeunittest clean)

