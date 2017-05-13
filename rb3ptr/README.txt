rb3ptr is a 3-pointer Red-black tree implementation that focuses on all of:
efficiency (time, space and also code-size), type safety, ease of use.

It aims for C99 compatibility. It uses a few C99 features but currently seems
to compile with -std=gnu89 as well. (Not: -std=c89 since we need "inline")

Type safety is provided by macros to wrap generic code into code that works
only for a specific type of node values. The generic code is not templated, so
a single compilation of it can be shared between all wrappers. The wrappers
machine code size comes mostly from inlined comparison functions.

rb3ptr has no external dependencies. It is even believed to not have any C
standard library dependencies.

Building the source
===================

This library source package must be built by calling build.sh. Build
dependencies: python3. build.sh produces build/include/rb3ptr.h which is the
file you want to include in your projects.

To test the build you can then run build-examples.sh.

Instead of building you can also use the prebuilt version in prebuilt/. Check
the git commit hash to make sure that it's a recent version.

Documentation
=============

Currently the best documentation is the example programs under examples/. Or
just look at the source or generated header file. The user-facing API has some
comments.
