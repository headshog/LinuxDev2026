CC = cc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O0 -g
GDB = gdb
GDBFLAGS = --batch --quiet --nx --return-child-result

PREFIX ?= /usr
bindir ?= $(PREFIX)/bin
DESTDIR ?=

ARCHIVE = range-0.1.0.tar.gz
DISTFILES = range.c Makefile range.spec multiples.gdb interval.gdb \
	multiples.expected interval.expected
TEST_OUTPUTS = multiples.log multiples.out interval.log interval.out

all: range

range: range.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

test: range
	LC_ALL=C $(GDB) $(GDBFLAGS) -x multiples.gdb ./range > multiples.log 2>&1 || { cat multiples.log; exit 1; }
	grep '^@@@' multiples.log > multiples.out
	diff -u multiples.expected multiples.out
	LC_ALL=C $(GDB) $(GDBFLAGS) -x interval.gdb ./range > interval.log 2>&1 || { cat interval.log; exit 1; }
	grep '^@@@' interval.log > interval.out
	diff -u interval.expected interval.out
	@echo 'GDB scenarios: OK'

install: range
	install -d "$(DESTDIR)$(bindir)"
	install -m 755 range "$(DESTDIR)$(bindir)/range"

dist: $(DISTFILES)
	tar -czf $(ARCHIVE) -C .. $(addprefix 05_Debugging/,$(DISTFILES))

clean:
	rm -f range $(TEST_OUTPUTS) $(ARCHIVE)

.PHONY: all test install dist clean
