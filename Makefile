CFLAGS=-O2 -std=c89 -Wall -D_POSIX_C_SOURCE=200809L

includes= avltree/sil_avltree.h
objects = bench.o sil_avl_bench.o timer/sil_timer.o rbtree_bench.o


bench: $(objects) $(includes)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -o $@ $(objects)

bench.o: bench.c bench.h
	$(CC) -c $(CFLAGS) $(EXTRA_CFLAGS) -o $@ $<

rbtree_bench.o: rbtree_bench.c rbtree_bench.h
	$(CC) -c $(CFLAGS) $(EXTRA_CFLAGS) -o $@ $<

sil_avl_bench.o: sil_avl_bench.c sil_avl_bench.h
	$(CC) -c $(CFLAGS) $(EXTRA_CFLAGS) -o $@ $<

timer/sil_timer.o: timer/sil_timer.c timer/sil_timer.c
	$(CC) -c $(CFLAGS) $(EXTRA_CFLAGS) -o $@ $<



runtests: bench
	./bench

clean:
	rm -f bench $(objects)

all: runtests



.PHONY: test clean
