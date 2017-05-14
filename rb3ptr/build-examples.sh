#!/bin/sh

# NOTE: be sure to ./build.sh first!

set -e

if ! [ -e "$0" ] || ! [ -d templates ]; then
	echo >&2 "ERROR: We're probably not in the right directory to build rb3ptr"
	echo >&2 "ERROR: Refusing operation"
	exit 1
fi

cc -std=c99 -O2 -Wall -Wextra -Ibuild/include examples/simple.c -o build/simple-example
cc -std=c99 -O2 -Wall -Wextra -Ibuild/include examples/simple-typesafe.c -o build/simple-typesafe-example
cc -std=c99 -O2 -Wall -Wextra -Ibuild/include examples/simple-typesafe-bsd-shim.c -o build/simple-typesafe-example
