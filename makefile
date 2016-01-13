CFLAGS=-Wall -std=c99
LDLIBS=-lm
x.bezier: bezier.c
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@
