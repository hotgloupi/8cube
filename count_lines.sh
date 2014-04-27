#!/bin/sh
# @file count_lines.sh
# @author <raphael.londeix@gmail.com> Raphael Londeix
# @version @FIXME@

SCRIPTCMD="$0"
SCRIPT=`basename "$0"`
SCRIPT_DIR=`python -c "import os;print(os.path.abspath(os.path.dirname('$0')))"`

abspath()
{
	python -c 'import os, sys; print(os.path.abspath(os.path.join(*((len(sys.argv) > 1) and sys.argv[1:] or ["."]))))' "$@"
}

fatal()
{
	echo "$*" 2>&1
	exit 1
}

usage()
{
	fatal "Usage: $SCRIPTCMD"
}

ROOT_DIR="${SCRIPT_DIR}"

find_src()
{
	find                            \
		"${ROOT_DIR}"/share         \
		"${ROOT_DIR}"/src/etc       \
		"${ROOT_DIR}"/src/cube      \
		"${ROOT_DIR}"/src/cubeapp   \
		"${ROOT_DIR}"/src/wrappers  \
		"${ROOT_DIR}"/src/cubeserv  \
		"${ROOT_DIR}"/src/launch    \
		"$@"
}

cpp_files()
{
	find_src -name '*.[ch]pp' -o -name '*.inl'
}

python_binding_files()
{
	find_src -name '*.py++'
}

python_files()
{
	find_src -name '*.py'
}

line_count()
{
	local lines files
	lines=$(cat `$1 | tr '\n' ' '` | grep -vE '^$' | wc -l)
	files=`$1 | wc -l`
	echo "$1: $lines lines in $files files"
}

line_count cpp_files
line_count python_files
line_count python_binding_files
