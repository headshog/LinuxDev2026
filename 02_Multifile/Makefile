CC = cc
AR = ar
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -fPIC
ARFLAGS = rcs

LIBRARIES = liboutput_static.a liboutput.so
PROGRAMS = prog prog-a prog-so
OBJECTS = prog.o fun.o const.o
TEST_OUTPUTS = \
	prog-noargs.out prog-a-noargs.out prog-so-noargs.out \
	prog-one.out prog-a-one.out prog-so-one.out \
	prog-three.out prog-a-three.out prog-so-three.out

all: $(LIBRARIES) $(PROGRAMS)

liboutput_static.a: fun.o const.o
	$(AR) $(ARFLAGS) $@ $^

liboutput.so: fun.o const.o
	$(CC) -shared $(LDFLAGS) -o $@ $^

prog: prog.o fun.o const.o
	$(CC) $(LDFLAGS) -o $@ $^

prog-a: prog.o liboutput_static.a
	$(CC) $(LDFLAGS) -o $@ $^

prog-so: prog.o liboutput.so
	$(CC) $(LDFLAGS) -o $@ prog.o -L. -loutput

prog.o fun.o: outlib.h

test: all
	./prog > prog-noargs.out 2>&1
	./prog-a > prog-a-noargs.out 2>&1
	LD_LIBRARY_PATH=. ./prog-so > prog-so-noargs.out 2>&1
	cmp prog-noargs.out prog-a-noargs.out
	cmp prog-noargs.out prog-so-noargs.out
	./prog one > prog-one.out 2>&1
	./prog-a one > prog-a-one.out 2>&1
	LD_LIBRARY_PATH=. ./prog-so one > prog-so-one.out 2>&1
	cmp prog-one.out prog-a-one.out
	cmp prog-one.out prog-so-one.out
	./prog one two three > prog-three.out 2>&1
	./prog-a one two three > prog-a-three.out 2>&1
	LD_LIBRARY_PATH=. ./prog-so one two three > prog-so-three.out 2>&1
	cmp prog-three.out prog-a-three.out
	cmp prog-three.out prog-so-three.out

clean:
	rm -f $(OBJECTS) $(LIBRARIES) $(PROGRAMS) $(TEST_OUTPUTS)

.PHONY: all test clean
