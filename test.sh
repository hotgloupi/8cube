#!/bin/sh
SCRIPT_CMD="$0"

realpath()
{
	python -c 'import os, sys; print(os.path.abspath(sys.argv[1]))' "$1"
}

fatal()
{
	echo "$*" 2>&1
	exit 1
}

usage()
{
	fatal "Usage: $SCRIPT_CMD build_dir"
}

[ -z "$1" ] && usage
[ -d "$1" ] || fatal "Error: $1: not a directory"

SOURCE_DIR=`dirname "$0"`
BUILD_DIR=`realpath "$1"`

PYTHON="${BUILD_DIR}/dependencies/python33/gcc/release/no-pymalloc/install/bin/python3"

make -C "$BUILD_DIR"
TO_TEST="cube cubeapp"
for dir in ${TO_TEST}
do
	${PYTHON} \
		-m unittest \
		discover \
		-s ${BUILD_DIR}/release/lib/python/${dir} \
		-t ${BUILD_DIR}/release/lib/python \
		-p '*.py' \
		-v
done
