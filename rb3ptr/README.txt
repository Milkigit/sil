rb3ptr is a 3-pointer Red-black tree implementation that focuses on all of:
efficiency (time, space and also code-size), type safety, ease of use.

It's an intrusively linked tree (memory allocation done by the client) and
includes a shim for the BSD <sys/tree.h> API. The BSD tree is almost equivalent
in API terms, but less efficient, both in code and data structure size and
generally in performance as well.

It aims for C99 compatibility. It uses a few C99 features but currently seems
to compile with -std=gnu89 as well. (Not: -std=c89 since we use "inline")

Type safety is provided by a thin macro layer that wraps the generic code. The
wrapper's machine code size comes mostly from inlined comparison functions
(inlining the comparison functions will soon be optional).

rb3ptr has no external dependencies. It's not even dependent on the C standard
library.

Implementation notes can be found at http://jstimpfle.de/blah/rbtree/main.html

Building the source
===================

The C source must be generated from the files under templates/. There are
pre-built files in prebuilt/ - check the git commit hash to make sure that it's
a recent version!

To build the source yourself, call build.sh (requires python3) to produce
build/include/rb3ptr.h. This is the file you want to include in your projects.

To test the build you can then run build-examples.sh.

RB3_GEN_IMPL() alternative: Additionally to rb3ptr.h (which can act as a single
header library), the generic tree implementation is generated as the standalone
files rb3ptr-impl.c and rb3ptr-impl.h. Compiling the implementation separately
has the disadvantage of having to drop multiple files into one's project, but
the advantage of not needing the RB3_GEN_IMPL() macro which is a pain for
debugging (client code - the tree code hopefully works fine!).

Documentation
=============

There is the beginning of a manpage: rb3ptr(3).
You can also look at the example programs under examples/.
Or just look at the source or generated header file. The user-facing API has
some comments.
