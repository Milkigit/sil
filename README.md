This is my personal collection of data structures implemented in C. It's likely
to grow.

When it gets a website someday, I will host it under the name sil.ly
(actually no, I already checked and it's not available: DNS name too short)

The following data structures currently exist (currently only trees):

Trees
=====

The trees are benchmarked, run build.sh and then ./bench to see result (or see
listing below). Additionally SGI STL std::set and Niels Provos' BSD Rb tree are
in the bench for comparison.

The bench can be built with different test data sizes, it only requires a
little editing in the right places.

avltree/

 - Bad AVL implementation. AVL trees are mostly inferior to Red-black trees,
   and this one is a really unusuable macro implementation. Don't use it.

rb2ptr/

 - A 2-pointer Red-black tree with color bits stuffed in the unused pointer
   bits (so only 2 pointer overhead per node). That means no parent pointers,
   so prior to deletion one has to find a node and collect the path to the node
   in a stack. That has advantages and disadvantages, but generally it's quite
   a fast implementation (it scores quite favourable to NP BSD Rb tree).

   It still needs some polish to be really usable.

rb3ptr/

 - A 3-pointer Red-black tree with color bits stuffed in the unused pointer
   bits (so only 3 pointer overhead per node). It's in pretty good shape and
   quite fast!


BENCH OUTPUTS:
==============

Test system: Debian stretch, GCC -O2 -DNDEBUG, AMD64 X2 250, 4G RAM

Tested with 2^20 elements of different sizes.

 - BENCH_PAYLOAD_TYPE=0 means (at the time of writing this) 1 int (4 byte) payload
 - BENCH_PAYLOAD_TYPE=1 means 2 doubles (16 byte) payload
 - BENCH_PAYLOAD_TYPE=2 means 10 doubles (80 byte) payload

Only the first int or double in each payload carries a nonzero value. This is
sufficient for discrimating the nodes and to see if there are different
tradeoffs regarding cache efficiency.

```
BENCH_PAYLOAD_TYPE=0

Generating 1048576 elements
Randomly permuting elements

Running bench STL std::set
===========================
Inserting each element: 0.883s
Finding each element: 0.771s
Removing each element: 0.983s

Running bench BSD RB tree
===========================
Inserting each element: 0.804s
Finding each element: 0.661s
Removing each element: 0.899s

Running bench SIL AVLtree
===========================
sizeof SIL_AVLhead struct: 26
position of payload in node: 28
sizeof struct sil_node: 32
Inserting each element: 0.732s
Finding each element: 0.624s
Removing each element: 0.786s

Running bench SIL 2-pointer Red-black tree
===========================
sizeof sil_rb_head struct: 16
position of payload in node: 16
sizeof struct silrb_node: 24
Inserting each element: 0.694s
Finding each element: 0.580s
Removing each element: 0.766s

Running bench SIL 3-pointer Red-black tree
===========================
sizeof rb3_head struct: 24
position of payload in node: 24
sizeof struct bench_rb3_node: 32
Inserting each element: 0.714s
Finding each element: 0.637s
Removing each element: 0.808s
```

```
BENCH_PAYLOAD_TYPE=1

Generating 1048576 elements
Randomly permuting elements

Running bench STL std::set
===========================
Inserting each element: 1.075s
Finding each element: 0.836s
Removing each element: 1.150s

Running bench BSD RB tree
===========================
Inserting each element: 0.896s
Finding each element: 0.746s
Removing each element: 0.956s

Running bench SIL AVLtree
===========================
sizeof SIL_AVLhead struct: 26
position of payload in node: 32
sizeof struct sil_node: 48
Inserting each element: 0.859s
Finding each element: 0.741s
Removing each element: 0.874s

Running bench SIL 2-pointer Red-black tree
===========================
sizeof sil_rb_head struct: 16
position of payload in node: 16
sizeof struct silrb_node: 32
Inserting each element: 0.826s
Finding each element: 0.710s
Removing each element: 0.865s

Running bench SIL 3-pointer Red-black tree
===========================
sizeof rb3_head struct: 24
position of payload in node: 24
sizeof struct bench_rb3_node: 40
Inserting each element: 0.824s
Finding each element: 0.735s
Removing each element: 0.885s
```

```
BENCH_PAYLOAD_TYPE=2

Generating 1048576 elements
Randomly permuting elements

Running bench STL std::set
===========================
Inserting each element: 1.069s
Finding each element: 0.838s
Removing each element: 1.152s

Running bench BSD RB tree
===========================
Inserting each element: 0.936s
Finding each element: 0.786s
Removing each element: 1.019s

Running bench SIL AVLtree
===========================
sizeof SIL_AVLhead struct: 26
position of payload in node: 32
sizeof struct sil_node: 56
Inserting each element: 0.906s
Finding each element: 0.776s
Removing each element: 0.909s

Running bench SIL 2-pointer Red-black tree
===========================
sizeof sil_rb_head struct: 16
position of payload in node: 16
sizeof struct silrb_node: 40
Inserting each element: 0.851s
Finding each element: 0.720s
Removing each element: 0.881s

Running bench SIL 3-pointer Red-black tree
===========================
sizeof rb3_head struct: 24
position of payload in node: 24
sizeof struct bench_rb3_node: 48
Inserting each element: 0.826s
Finding each element: 0.736s
Removing each element: 0.883s
```
