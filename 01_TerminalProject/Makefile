CC = cc
PKG_CONFIG = pkg-config
CPPFLAGS = $(shell $(PKG_CONFIG) --cflags ncursesw)
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic
LDLIBS = $(shell $(PKG_CONFIG) --libs ncursesw)

PREFIX ?= /usr
bindir ?= $(PREFIX)/bin
DESTDIR ?=

all: Show

Show: Show.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $< -o $@ $(LDLIBS)

install: Show
	install -d "$(DESTDIR)$(bindir)"
	install -m 755 Show "$(DESTDIR)$(bindir)/Show"

.PHONY: all install clean
clean:
	rm -f Show
