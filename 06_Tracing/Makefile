CC = cc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2 -g

PREFIX ?= /usr
bindir ?= $(PREFIX)/bin
libdir ?= $(PREFIX)/lib
DESTDIR ?=

ARCHIVE = safemove-0.1.0.tar.gz
DISTFILES = move.c protect.c safemove.in test.sh Makefile safemove.spec README.md

all: move libprotect.so safemove

move: move.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

libprotect.so: protect.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -fPIC -shared $(LDFLAGS) \
		-Wl,-soname,libprotect.so -o $@ $< -ldl

safemove: safemove.in Makefile
	sed -e 's|@move@|$(CURDIR)/move|g' \
		-e 's|@library@|$(CURDIR)/libprotect.so|g' $< > $@
	chmod +x $@

test: all
	sh test.sh

install: all
	install -d "$(DESTDIR)$(bindir)" "$(DESTDIR)$(libdir)/safemove"
	install -m 755 move "$(DESTDIR)$(bindir)/move"
	install -m 755 libprotect.so "$(DESTDIR)$(libdir)/safemove/libprotect.so"
	sed -e 's|@move@|$(bindir)/move|g' \
		-e 's|@library@|$(libdir)/safemove/libprotect.so|g' safemove.in \
		> "$(DESTDIR)$(bindir)/safemove"
	chmod 755 "$(DESTDIR)$(bindir)/safemove"

dist: $(DISTFILES)
	tar -czf $(ARCHIVE) -C .. $(addprefix 06_Tracing/,$(DISTFILES))

srpm: dist
	rpmbuild --define "_sourcedir $(CURDIR)" \
		--define "_srcrpmdir $(CURDIR)" -bs safemove.spec

clean:
	rm -f move libprotect.so safemove $(ARCHIVE)

.PHONY: all test install dist srpm clean
