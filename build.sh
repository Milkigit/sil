(cd rb3ptr; sh build.sh)

defines="
	-DNDEBUG
	-DBENCH_PAYLOAD_TYPE=0
	-D_POSIX_C_SOURCE=200809L
"

c++ -O2 $defines -std=c++11 -Wall -Wextra -c bench_stl_set.cpp

# This was originally a C program, but since we added the std::set bench we
# have to link in libstdc++. Let's hope that works. NOTE: it will crash badly
# when the C++ code throws exceptions (std::bad_alloc!).
cc -std=c99 -O2 \
	-Wall -Wextra -Wno-missing-braces \
	$defines \
	-l stdc++ \
	-o bench \
	bench.c \
	bench_np_rbtree.c \
	bench_sil_avltree.c \
	bench_sil_rb2ptr.c \
	-Irb3ptr/build/include/ \
	bench_sil_rb3ptr.c \
	timer/sil_timer.c \
	rb2ptr/sil_rb2ptr.c \
	\
	bench_stl_set.o
