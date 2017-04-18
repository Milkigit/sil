CFLAGS=-O2 -std=c89 -Wall -D_POSIX_C_SOURCE=200809L

includes= avltree/sil_avltree.h
objects = bench.o bench_sil_avltree.o timer/sil_timer.o bench_np_rbtree.o


bench: $(objects) $(includes)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -o $@ $(objects)

bench.o: bench.c bench.h
	$(CC) -c $(CFLAGS) $(EXTRA_CFLAGS) -o $@ $<

bench_np_rbtree.o: bench_np_rbtree.c bench_np_rbtree.h
	$(CC) -c $(CFLAGS) $(EXTRA_CFLAGS) -o $@ $<

bench_sil_avltree.o: bench_sil_avltree.c bench_sil_avltree.h
	$(CC) -c $(CFLAGS) $(EXTRA_CFLAGS) -o $@ $<

timer/sil_timer.o: timer/sil_timer.c timer/sil_timer.c
	$(CC) -c $(CFLAGS) $(EXTRA_CFLAGS) -o $@ $<



runtests: bench
	./bench

clean:
	rm -f bench $(objects)

all: runtests



.PHONY: test clean
