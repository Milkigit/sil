cc -O2 -DNDEBUG -std=c89 -Wall -Wextra -D_POSIX_C_SOURCE=200809L -o bench \
	bench.c \
	bench_np_rbtree.c \
	bench_sil_avltree.c \
	bench_sil_rb2ptr.c \
	bench_sil_rb3ptr.c \
	timer/sil_timer.c \
	rb2ptr/sil_rb2ptr.c \
	rb3ptr/rb3ptr.c
