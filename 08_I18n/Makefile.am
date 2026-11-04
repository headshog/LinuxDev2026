SUBDIRS = po
ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = guess-number
guess_number_SOURCES = guess-number.c
AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\"
AM_CFLAGS = -std=c11 -Wall -Wextra -Wpedantic
LDADD = $(LIBINTL)

TEST_EXTENSIONS = .py
PY_LOG_COMPILER = $(PYTHON)
TESTS = test.py
TESTS_ENVIRONMENT = GUESS_NUMBER="$(abs_builddir)/guess-number"

EXTRA_DIST = guess-number.spec README.md test.py

srpm: dist
	rpmbuild --define "_sourcedir $(abs_builddir)" \
		--define "_srcrpmdir $(abs_srcdir)" -bs $(srcdir)/guess-number.spec

.PHONY: srpm
