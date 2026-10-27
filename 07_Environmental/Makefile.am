bin_PROGRAMS = rhasher
rhasher_SOURCES = rhasher.c
AM_CFLAGS = -Wall -Wextra

TEST_EXTENSIONS = .sh
SH_LOG_COMPILER = $(SHELL)
TESTS = test.sh

EXTRA_DIST = test.sh rhasher.spec

# "make maintainer-clean" leaves only the hand-written sources
MAINTAINERCLEANFILES = $(DIST_ARCHIVES) \
	$(srcdir)/Makefile.in $(srcdir)/aclocal.m4 $(srcdir)/configure \
	$(srcdir)/config.h.in $(srcdir)/config.h.in~

distclean-local:
	-rm -rf ./$(DEPDIR)

maintainer-clean-local:
	-rm -rf $(srcdir)/build-aux

srpm: dist
	rpmbuild --define "_sourcedir $(abs_builddir)" \
		--define "_srcrpmdir $(abs_srcdir)" -bs $(srcdir)/rhasher.spec

.PHONY: srpm
