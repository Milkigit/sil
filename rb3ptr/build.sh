#!/bin/sh

set -e

if ! [ -d header ] || ! [ -d impl ] || ! [ -d templates ]; then
	echo >&2 "ERROR: We're probably not in the right directory to build rb3ptr"
	echo >&2 "ERROR: Refusing operation"
	exit 1
fi

rm -rf build
mkdir -p build/include
python3 dist/gen-header.py > build/include/rb3ptr.h
python3 dist/gen-macros.py > build/include/rb3ptr-gen.h
python3 dist/gen-impl.py > build/rb3ptr.c

cc -std=c99 -O2 -Ibuild/include build/rb3ptr.c examples/simple.c -o build/simple-example
cc -std=c99 -O2 -Ibuild/include build/rb3ptr.c examples/simple-typesafe.c -o build/simple-typesafe-example
