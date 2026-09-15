CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic
LDLIBS = -lncursesw

Show: Show.c
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)

.PHONY: clean
clean:
	rm -f Show
