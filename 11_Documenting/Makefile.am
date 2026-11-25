SUBDIRS = po
ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I$(srcdir) -DLOCALEDIR=\"$(localedir)\"
AM_CFLAGS = -std=c11 -Wall -Wextra -Wpedantic

bin_PROGRAMS = guess-number
guess_number_SOURCES = guess-number.c roman.c roman.h
LDADD = $(LIBINTL)

check_PROGRAMS = test_roman
test_roman_SOURCES = test_roman.c roman.c roman.h
TEST_EXTENSIONS = .py
PY_LOG_COMPILER = $(PYTHON)
TESTS = test_roman test.py
TESTS_ENVIRONMENT = GUESS_NUMBER="$(abs_builddir)/guess-number"

nodist_man_MANS = guess-number.1
EXTRA_DIST = test.py README.md guess-number.spec Doxyfile.in
CLEANFILES = mainpage.dox mainpage.dox.tmp guess-number.1

all-local: docs/html/index.html guess-number.1

mainpage.dox: guess-number$(EXEEXT)
	LC_ALL=C LANGUAGE=C ./guess-number --help-md > $@.tmp
	mv $@.tmp $@

docs/html/index.html: Doxyfile mainpage.dox $(srcdir)/guess-number.c $(srcdir)/roman.c $(srcdir)/roman.h
	$(DOXYGEN) Doxyfile

guess-number.1: guess-number$(EXEEXT)
	LC_ALL=C LANGUAGE=C $(HELP2MAN) --no-info \
		--name='guess a number using binary search and Roman numerals' \
		--output=$@ ./guess-number

install-data-local: docs/html/index.html
	$(MKDIR_P) "$(DESTDIR)$(docdir)/html"
	cp -R docs/html/. "$(DESTDIR)$(docdir)/html/"

uninstall-local:
	rm -rf "$(DESTDIR)$(docdir)/html"

clean-local:
	rm -rf docs

srpm: dist
	rpmbuild --define "_sourcedir $(abs_builddir)" \
		--define "_srcrpmdir $(abs_srcdir)" -bs $(srcdir)/guess-number.spec

.DELETE_ON_ERROR:
.PHONY: srpm
