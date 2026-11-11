CC = cc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2 -g
PYTHON ?= python3

PREFIX ?= /usr
bindir ?= $(PREFIX)/bin
DESTDIR ?=

SIZE ?= 6
SYMBOLS ?= .\#
SEED ?= 42
export SIZE SYMBOLS SEED

PROGRAMS = maze maze-size maze-symbols maze-seed
GENERATED_SOURCES = maze-size.c maze-symbols.c maze-seed.c
PATCHES = 0001-size.patch 0002-symbols.patch 0003-seed.patch
ARCHIVE = maze-generator-0.1.0.tar.gz
DISTFILES = maze.c Makefile README.md test.py maze-generator.spec

all: $(PROGRAMS)

maze-size.c: maze.c 0001-size.patch
	patch --batch --fuzz=0 -o $@ $< < 0001-size.patch

maze-symbols.c: maze-size.c 0002-symbols.patch
	patch --batch --fuzz=0 -o $@ $< < 0002-symbols.patch

maze-seed.c: maze-symbols.c 0003-seed.patch
	patch --batch --fuzz=0 -o $@ $< < 0003-seed.patch

$(PROGRAMS): %: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

run: all
	./maze
	./maze-size "$$SIZE"
	./maze-symbols "$$SYMBOLS" "$$SIZE"
	./maze-seed "$$SEED" "$$SYMBOLS" "$$SIZE"

test: all
	$(PYTHON) test.py

install: all
	install -d "$(DESTDIR)$(bindir)"
	install -m 755 $(PROGRAMS) "$(DESTDIR)$(bindir)/"

dist: $(DISTFILES) $(PATCHES)
	tar -czf $(ARCHIVE) -C .. $(addprefix 09_PatchDiff/,$(DISTFILES))

srpm: dist
	rpmbuild --define "_sourcedir $(CURDIR)" \
		--define "_srcrpmdir $(CURDIR)" -bs maze-generator.spec

clean:
	rm -f $(PROGRAMS) $(GENERATED_SOURCES) $(ARCHIVE) \
		$(addsuffix .orig,$(GENERATED_SOURCES)) $(addsuffix .rej,$(GENERATED_SOURCES))

.DELETE_ON_ERROR:
.PHONY: all run test install dist srpm clean
