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
Inserting each element: 0.886s
Finding each element: 0.769s
Removing each element: 0.985s

Cache should be warm now, run again...

Running bench STL std::set
===========================
Inserting each element: 0.837s
Finding each element: 0.768s
Removing each element: 0.981s

Running bench BSD RB tree
===========================
Inserting each element: 0.806s
Finding each element: 0.658s
Removing each element: 0.895s

Running bench SIL 3-pointer Red-black tree
===========================
Inserting each element: 0.658s
Finding each element: 0.583s
Removing each element: 0.727s

Running bench SIL AVLtree
===========================
Inserting each element: 0.716s
Finding each element: 0.614s
Removing each element: 0.786s

Running bench SIL 2-pointer Red-black tree
===========================
sizeof sil_rb_head struct: 16
position of payload in node: 16
sizeof struct silrb_node: 24
Inserting each element: 0.687s
Finding each element: 0.573s
Removing each element: 0.761s
```

```
BENCH_PAYLOAD_TYPE=1

Generating 1048576 elements
Randomly permuting elements

Running bench STL std::set
===========================
Inserting each element: 1.058s
Finding each element: 0.833s
Removing each element: 1.146s

Cache should be warm now, run again...

Running bench STL std::set
===========================
Inserting each element: 1.010s
Finding each element: 0.838s
Removing each element: 1.138s

Running bench BSD RB tree
===========================
Inserting each element: 0.890s
Finding each element: 0.743s
Removing each element: 0.954s

Running bench SIL 3-pointer Red-black tree
===========================
Inserting each element: 0.807s
Finding each element: 0.719s
Removing each element: 0.875s

Running bench SIL AVLtree
===========================
Inserting each element: 0.855s
Finding each element: 0.740s
Removing each element: 0.874s

Running bench SIL 2-pointer Red-black tree
===========================
sizeof sil_rb_head struct: 16
position of payload in node: 16
sizeof struct silrb_node: 32
Inserting each element: 0.821s
Finding each element: 0.710s
Removing each element: 0.862s
```

```
BENCH_PAYLOAD_TYPE=2

Generating 1048576 elements
Randomly permuting elements

Running bench STL std::set
===========================
Inserting each element: 1.061s
Finding each element: 0.831s
Removing each element: 1.141s

Cache should be warm now, run again...

Running bench STL std::set
===========================
Inserting each element: 1.010s
Finding each element: 0.832s
Removing each element: 1.138s

Running bench BSD RB tree
===========================
Inserting each element: 0.933s
Finding each element: 0.785s
Removing each element: 0.998s

Running bench SIL 3-pointer Red-black tree
===========================
Inserting each element: 0.818s
Finding each element: 0.727s
Removing each element: 0.876s

Running bench SIL AVLtree
===========================
Inserting each element: 0.899s
Finding each element: 0.773s
Removing each element: 0.910s

Running bench SIL 2-pointer Red-black tree
===========================
sizeof sil_rb_head struct: 16
position of payload in node: 16
sizeof struct silrb_node: 40
Inserting each element: 0.846s
Finding each element: 0.715s
Removing each element: 0.877s
```
