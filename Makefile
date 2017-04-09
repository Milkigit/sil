CFLAGS = -O2 -std=c89 -Wall -D_POSIX_C_SOURCE=200809L

all: test

bench: bench.c avltree/sil_avltree.h timer/sil_timer.h timer/sil_timer.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -o bench bench.c timer/sil_timer.c

test: bench
	./bench

clean:
	rm -f bench

.PHONY: test clean
