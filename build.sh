# Note: Payload types 1 and 2 (structs of double-typed values) do not work yet
# for random binary-data reinterpreted structs.
# The permuted integer-valued sequence (benchtestdata_init()) is fine.
defines="
	-DNDEBUG
	-DBENCH_PAYLOAD_TYPE=0
	-D_POSIX_C_SOURCE=200809L
"

c++ -O2 $defines -std=c++11 -Wall -Wextra -c bench_stl_set.cpp

# This was originally a C program, but since we added the std::set bench we
# have to link in libstdc++. Let's hope that works. NOTE: it will crash badly
# when the C++ code throws exceptions (std::bad_alloc!).
cc -std=c89 -Wall -Wextra -O2 \
	$defines \
	-l stdc++ \
	-o bench \
	bench.c \
	bench_np_rbtree.c \
	bench_sil_avltree.c \
	bench_sil_rb2ptr.c \
	bench_sil_rb3ptr.c \
	timer/sil_timer.c \
	rb2ptr/sil_rb2ptr.c \
	rb3ptr/rb3ptr.c \
	\
	bench_stl_set.o
