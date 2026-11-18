ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I$(srcdir)
AM_CFLAGS = -std=c99 -Wall -Wextra -Wpedantic

lib_LTLIBRARIES = libgrowablebuf.la
libgrowablebuf_la_SOURCES = buf.c
libgrowablebuf_la_LDFLAGS = -version-info 0:0:0 -no-undefined \
	-export-symbols-regex '^buf_grow1$$'
pkginclude_HEADERS = buf.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = growable-buf.pc

check_PROGRAMS = check_buf
check_buf_SOURCES = check_buf.c
check_buf_CFLAGS = $(AM_CFLAGS) $(CHECK_CFLAGS)
check_buf_LDADD = libgrowablebuf.la $(CHECK_LIBS)
TESTS = check_buf

EXTRA_DIST = README.md UNLICENSE growable-buf.pc.in growable-buf.spec
CLEANFILES = *.gcov *.gcda *.gcno .libs/*.gcda .libs/*.gcno

if ENABLE_COVERAGE
coverage:
	$(MAKE) coverage-reset
	$(MAKE) check
	$(GCOV) -b -c -o .libs $(srcdir)/buf.c
	$(GCOV) -b -c check_buf-check_buf.gcno
else
coverage:
	@echo 'Configure a separate build with --enable-coverage first.' >&2
	@exit 1
endif

coverage-reset:
	rm -f .libs/buf.gcda check_buf-check_buf.gcda check_buf.log check_buf.trs test-suite.log

srpm: dist
	rpmbuild --define "_sourcedir $(abs_builddir)" \
		--define "_srcrpmdir $(abs_srcdir)" -bs $(srcdir)/growable-buf.spec

.PHONY: coverage coverage-reset srpm
