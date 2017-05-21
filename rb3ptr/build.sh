#!/bin/sh

set -e

if ! [ -e "$0" ] || ! [ -d templates ]; then
	echo >&2 "ERROR: We're probably not in the right directory to build rb3ptr"
	echo >&2 "ERROR: Refusing operation"
	exit 1
fi

rm -rf build
mkdir -p build/include
python3 dist/gen-macros.py > build/include/rb3ptr.h
cat bsd-shim.h > build/include/rb3ptr-bsd-shim.h

# build/rb3ptr-impl.{c,h}
sh dist/gen-impl.sh
