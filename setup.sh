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
[ -d "$1" ] || fatal "Error: $1: not a directory"

[ -z "$2" ] && usage
[ -e "$2" -a ! -d "$2" ] && fatal "Error: $2: already exists but not a directory"

BUILD_DIR="$1"
DEST_DIR="$2"

echo "-- Setup of $BUILD_DIR in $DEST_DIR"

RELEASE_DATE_FILE="$DEST_DIR/share/8cube/.release"
echo $RELEASE_DATE_FILE

[ -d "$DEST_DIR" ] && [ ! -f "$RELEASE_DATE_FILE" ] && \
	fatal "Error: $DEST_DIR: exists but not a release directory"

rm -rf "$DEST_DIR"
mkdir -p "`dirname $RELEASE_DATE_FILE`"
cp -rv "$BUILD_DIR/release"/* "$DEST_DIR"
find "$DEST_DIR" -name '*.o' -exec rm -fv {} \;
find "$DEST_DIR" -name '*.a' -exec rm -fv {} \;
find "$DEST_DIR" -name 'Tupfile' -exec rm -v {} \;
find "$DEST_DIR" -name '__pycache__' -exec rm -frv {} \;
find "$DEST_DIR" -executable -and -type f -exec strip -s {} \;
date > "$RELEASE_DATE_FILE"
echo "-- Release built at `cat $RELEASE_DATE_FILE`"
echo


[ -n "$(find "$DEST_DIR" \( -name 'libetc.so' -o -name 'libcube.so' -o -name 'libcubeapp.so' \))" ] && \
	echo "WARNING: release contains core dynamic libraries"
