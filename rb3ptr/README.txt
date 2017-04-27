rb3ptr is a 3-pointer Red-black tree implementation that focuses on all of:
efficiency (time, space and also code-size), type safety, ease of use.

It aims for C99 compatibility. It uses a few C99 features but currently seems
to compile with -std=gnu89 as well.

Type safety is provided by macros to wrap generic code into code that works
only for a specific type of node values. The generic code is not templated, so
a single compilation of it can be shared between all wrappers. The wrappers
machine code size comes mostly from inlined comparison functions.

Currently there are 2 header files and one C file (for the generic code) which
must be compiled separately. The goal is to provide a header-only version, too.

rb3ptr has no external dependencies. It is even believed to not have any C
standard library dependencies.

Building the source
===================

This library source package must be built by calling build.sh. Build
dependencies: python3. build.sh produces

  build/include/rb3ptr.h - generic include file
  build/include/rb3ptr-gen.h - contains macros to build typesafe wrappers
  build/rb3ptr.c - generic implementation (shared by typesafe wrappers)

Some examples are built as well...

Documentation
=============

So far the only documentation is in the header files. Build the source package
and look in rb3ptr.h. You can also read without building by looking in the
header sources under header/.
