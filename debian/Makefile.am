## Process this file with automake to produce Makefile.in

EXTRA_DIST = changelog control copyright \
	rules seam-dev.install seam.install seam.info seam.spec

dist-hook:
	$(AMTAR) -c $(EXTRA_DIST) | \
	(cd $(distdir) && $(AMTAR) x)
