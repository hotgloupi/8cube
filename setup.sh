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

export DEST_DIR
export BUILD_DIR

echo "-- Setup of $BUILD_DIR in $DEST_DIR"

RELEASE_DATE_FILE="$DEST_DIR/share/8cube/.release"

[ -d "$DEST_DIR" ] && [ ! -f "$RELEASE_DATE_FILE" ] && \
	fatal "Error: $DEST_DIR: exists but not a release directory"

rm -rf "$DEST_DIR"
mkdir -p "`dirname $RELEASE_DATE_FILE`" 2>&1 > /dev/null
RELEASE_LOG="${DEST_DIR}"/.release.log

echo "Starting release log file at `date`" > "${RELEASE_LOG}"

log()
{
	echo "--> $*" 2>&1 | tee -a "${RELEASE_LOG}"
}

cmd()
{
	echo "[COMMAND] $@" >> "${RELEASE_LOG}"
	"$@" 2>&1 >> "${RELEASE_LOG}" || fatal "Command failed"
}

log "Copying release files"
cmd cp -rv "$BUILD_DIR/release"/* "$DEST_DIR"

find_python_home()
{
    python3 << EOF
import sys
import os
import subprocess
import shutil

def cmd(*args, **kwargs):
    return subprocess.check_output(list(args), **kwargs).decode('utf8')

DEST_DIR = os.path.abspath(os.environ["DEST_DIR"])
BUILD_DIR = os.path.abspath(os.environ["BUILD_DIR"])
for lib in cmd('find', BUILD_DIR, '-name', "python3.3").split('\n'):
    if '/install/lib/' in lib:
        print(lib)
        break
EOF
}

log "Copy python files"
BUILD_PYTHON_HOME=$(find_python_home)
cmd cp -rvf "${BUILD_PYTHON_HOME}" "$DEST_DIR"/lib
PYTHON_HOME="${DEST_DIR}/lib/python3.3"
log "Removing unused python files"
cmd rm -rvf "$PYTHON_HOME"/disutils
cmd rm -rvf "$PYTHON_HOME"/test

log "Deleting useless files"
cmd find "$DEST_DIR" -name '*.o' -print -delete
cmd find "$DEST_DIR" -name '*.a' -print -delete
cmd find "$DEST_DIR" -name '*.pyc' -print -delete
cmd sh -c "find \"$DEST_DIR\" \
	-ignore_readdir_race \
	-type d \
	-name '__pycache__' \
	-print \
	-exec rm -rf {} \; || exit 0"
cmd sh -c "find \"$DEST_DIR\" \
	-ignore_readdir_race \
	-type d \
	-name '__pycache__' \
	-print \
	-exec rm -rf {} \; || exit 0"

cmd find "$DEST_DIR" -name 'Tupfile' -print -delete
cmd find $DEST_DIR -name '*~' -print -delete

log "Patching libraries and executables"
patch_libs()
{
    python3 << EOF
import sys
import os
import subprocess
import shutil
import stat
from pprint import pprint

def cmd(*args, **kwargs):
    print(*args, **kwargs)
    return subprocess.check_output(list(args), **kwargs)

def error(*args):
    sys.stderr.write(' '.join(str(a) for a in args))

def fatal(*args):
    error(*args)
    sys.exit(1)

def which(executable):
    path = os.environ["PATH"]
    for d in path.split(os.pathsep):
        file = os.path.join(d,executable)
        if os.path.exists(file):
            return file

if which("otool"):
    def get_binary_dependencies(arg):
        deps = cmd('otool', '-L', arg).decode("utf-8")
        deps = deps.split("\n")[:-1]
        if not deps:
            return []
        return (
            dep.strip().split(" ")[0] for dep in deps[1:]
        )
elif which("ldd"):
    def get_binary_dependencies(arg):
        deps = cmd('ldd', arg).decode("utf-8")
        deps = deps.split('\n')
        if not deps:
            return
        for dep in deps[1:]:
            dep = dep.strip()
            if not dep or '=>' not in dep:
                return
            yield dep.split('=>')[1].strip().split(' ')[0].strip()
else:
    raise Exception("otool or ldd needed")

DEST_DIR = os.path.abspath(os.environ["DEST_DIR"])
BUILD_DIR = os.path.abspath(os.environ["BUILD_DIR"])
def is_binary(path):
    if path.endswith('.py') or path.endswith('.sh') or path.endswith('.bat'):
        return False
    if not stat.S_IXUSR & os.stat(path)[stat.ST_MODE]:
        return False
    with open(path, 'rb') as f:
        if f.read(2) == b'#!':
            return False
    return True

BINARIES = [
    binary.strip() for binary in cmd(
        'find', DEST_DIR, '-executable', '-and', '-type', 'f', '-print'
    ).decode('utf8').split('\n')
    if binary.strip() and is_binary(binary) and 'lib-dynload' not in binary
]


def to_add(dep):
    return not (
        dep.startswith('/usr') or
        dep.startswith('/lib') or
        dep.startswith(DEST_DIR)
    )

def fill_dependencies(dependencies, binary_path, indent=1):
    #print('-' * indent, binary_path)
    children = dependencies.setdefault(binary_path, set())
    l_dep = list(get_binary_dependencies(binary_path))
    for dependency in l_dep:
        if '@executable_path' in dependency:
            continue
        #print('-'*indent + '> dep', dependency)
        if not to_add(dependency):
            #print('-'*indent + '> IGNORE', dependency)
            continue
        children.add(dependency)
        if dependency not in dependencies:
            fill_dependencies(dependencies, dependency, indent + 1)

DEPENDENCIES = {}
for bin in BINARIES:
    fill_dependencies(DEPENDENCIES, bin)

#pprint(DEPENDENCIES)

DEST_LIB_DIR = os.path.join(DEST_DIR, 'lib')
SHIPPED_DEPENDENCIES = {}
for k in DEPENDENCIES.keys():
    if not k.startswith(DEST_DIR):
        dest = os.path.join(DEST_LIB_DIR, os.path.basename(k))
        SHIPPED_DEPENDENCIES[k] = dest
        cmd('cp', '-fv', k, dest)

#pprint(SHIPPED_DEPENDENCIES)

for dep in DEPENDENCIES:
    if dep in SHIPPED_DEPENDENCIES:
        dep = SHIPPED_DEPENDENCIES[dep]
    reldir = os.path.relpath(DEST_LIB_DIR, start = os.path.dirname(dep))
    cmd(
        'patchelf',
        #'--debug',
        '--set-rpath', os.path.join('$' + 'ORIGIN', reldir),
        dep
    )

TO_STRIP = list(
    binary.strip() for binary in cmd(
        'find', DEST_DIR, '-executable', '-and', '-type', 'f', '-print'
    ).decode('utf8').split('\n')
    if binary.strip() and is_binary(binary)
)

for bin in TO_STRIP:
    try:
        cmd('strip', '-s', bin)
    except:
        print("Couldn't strip", bin)
EOF
}

cmd patch_libs


log "Ending release at `date`"
date > "$RELEASE_DATE_FILE"

echo "-- Release built at `cat $RELEASE_DATE_FILE`"

