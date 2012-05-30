#!/bin/sh
# @file setup.sh
# @author <raphael.londeix@gmail.com> Raphael Londeix

SCRIPTCMD="$0"
SCRIPT=`basename "$0"`
SCRIPT_DIR=`python3 -c "import os;print(os.path.abspath(os.path.dirname('$0')))"`

abspath()
{
	echo `python3 -c "import os;print(os.path.abspath('$1'))"`
}

fatal()
{
	echo "$*" 2>&1
	exit 1
}

usage()
{
	fatal "Usage: $SCRIPTCMD build_dir release_dir"
}

[ -z "$1" ] && usage
[ -d "$1" ] || fatal "Error: $1 is not a directory"

[ -z "$2" ] && usage
[ -e "$2" -a ! -d "$2" ] && fatal "Error: $2 exists and is not a directory"

BUILD_DIR="$1"
DEST_DIR="$2"

echo "-- Setup of $BUILD_DIR in $DEST_DIR"

mkdir -p "$DEST_DIR"
cp -rv "$BUILD_DIR/release"/* "$DEST_DIR"
find "$DEST_DIR" -name '*.o' -exec rm -fv {} \;
find "$DEST_DIR" -name '*.a' -exec rm -fv {} \;
find "$DEST_DIR" -name 'Tupfile' -exec rm -v {} \;
find "$DEST_DIR" -name '__pycache__' -exec rm -frv {} \;
find "$DEST_DIR" -executable -and -type f -exec strip -s {} \;

