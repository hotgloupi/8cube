#!/usr/bin/env python3

import os, sys
import subprocess
import shutil
from datetime import datetime
from os.path import join, isdir, isfile, dirname, abspath, exists
import stat
from pprint import pprint

SCRIPT = abspath(__file__)
SCRIPT_DIR = dirname(SCRIPT)
SCRIPT_CMD = sys.argv[0]

sys.path.append(join(SCRIPT_DIR, '.config', 'tupcfg', 'src'))
from tupcfg.tools import which

IS_WINDOWS = "win32" in sys.platform.lower()
IS_OSX = "darwin" in sys.platform.lower()

def fatal(*args):
    print(*args, file = sys.stderr)
    sys.exit(1)

def usage():
    fatal("Usage:", SCRIPT_CMD, "build-dir release-dir")

if len(sys.argv) != 3:
    usage()

BUILD_DIR, DEST_DIR = sys.argv[1], sys.argv[2]
if not isdir(BUILD_DIR):
    fatal("Error: ", BUILD_DIR, ": not a directory")

if exists(DEST_DIR) and not isdir(DEST_DIR):
    fatal("Error: $2: already exists but not a directory")




RELEASE_DATE_FILE = join(DEST_DIR, ".release")

if isdir(DEST_DIR) and not isfile(RELEASE_DATE_FILE):
    fatal("Error:", DEST_DIR, ": exists but not a release directory")


###############################################################################
# Remove old release dir and create the marker.

shutil.rmtree(DEST_DIR, ignore_errors = True)
os.makedirs(dirname(RELEASE_DATE_FILE))
with open(RELEASE_DATE_FILE, 'w') as f:
    print(datetime.utcnow().ctime(), file = f)


###############################################################################
# Utilities.
RELEASE_LOG = join(DEST_DIR, ".release.log")
def log(*args):
    print(*args)
    with open(RELEASE_LOG, "a") as f:
        print("->", *args, file = f)

def cmd_output(*args, **kwargs):
    log('[COMMAND]', *args)
    with open(RELEASE_LOG, "a") as f:
        return subprocess.check_output(list(args), **kwargs).decode('utf8')

def cmd(*args, **kwargs):
    log('[COMMAND]', *args)
    with open(RELEASE_LOG, "a") as f:
        return subprocess.check_call(
            list(args),
            stdout = f,
            stderr = subprocess.PIPE,
            **kwargs
        )

def is_binary(path):
    if IS_WINDOWS and any(path.lower().endswith(ext) for ext in ['.exe', '.dll', '.pyd']):
        return True
    if path.endswith('.py') or path.endswith('.sh') or path.endswith('.bat'):
        return False
    if not stat.S_IXUSR & os.stat(path)[stat.ST_MODE]:
        return False
    with open(path, 'rb') as f:
        if f.read(2) == b'#!':
            return False
    return True

if which("otool"):
    def get_binary_dependencies(arg):
        deps = cmd_output('otool', '-L', arg)
        deps = deps.split("\n")[:-1]
        if not deps:
            return []
        return (
            dep.strip().split(" ")[0] for dep in deps[1:]
        )
elif which("ldd"):
    def get_binary_dependencies(arg):
        deps = cmd_output('ldd', arg)
        deps = deps.split('\n')
        if not deps:
            return
        for dep in deps[1:]:
            dep = dep.strip()
            if not dep or '=>' not in dep:
                return
            yield dep.split('=>')[1].strip().split(' ')[0].strip()
elif not IS_WINDOWS:
    raise Exception("otool or ldd needed")

if IS_OSX:
    def strip(bin):
        cmd('strip', '-S', '-x', bin)
elif IS_WINDOWS:
    def strip(bin):
        cmd('strip', '-s', bin)
else:
    def strip(bin):
        cmd('chmod', 'u+w', bin)
        cmd('strip', '-s', bin)

###############################################################################
# Initial log.
log("Starting release of ", BUILD_DIR, "in", DEST_DIR, 'at', datetime.utcnow().ctime())

###############################################################################
log("Copying release files")
BUILD_RELEASE_DIR = join(BUILD_DIR, 'release')
for d in os.listdir(BUILD_RELEASE_DIR):
    log(" - Copying", join(BUILD_RELEASE_DIR, d), "in", DEST_DIR)
    shutil.copytree(join(BUILD_RELEASE_DIR, d), join(DEST_DIR, d))


###############################################################################
# Find python home and copy it in the release
for PYTHON_HOME in [
    join(BUILD_DIR, 'dependencies/python3.3/install/lib/python3.3'),
    "c:/Python33",
    None
]:
    if exists(PYTHON_HOME):
        break

assert PYTHON_HOME is not None


DEST_PYTHON_HOME = join(DEST_DIR, "lib/python3.3")
remove_packages = [
    'distutils',
    'test',
    'turtledemo',
    'site-packages',
    'lib2to3',
]
log("Copying python files in", DEST_PYTHON_HOME)
os.makedirs(DEST_PYTHON_HOME)
if IS_WINDOWS:
    for d in ['DLLs', 'Lib']:
        log(" - Copying", join(PYTHON_HOME, d), "in", join(DEST_PYTHON_HOME, d))
        shutil.copytree(
            join(PYTHON_HOME, d),
            join(DEST_PYTHON_HOME, d)
        )
    for p in remove_packages:
        p = join(DEST_PYTHON_HOME, 'Lib', p)
        log(" - Removing package", p)
        shutil.rmtree(p, ignore_errors = True)
else:
    assert False # TODO
PYTHON_HOME = DEST_PYTHON_HOME
del DEST_PYTHON_HOME


###############################################################################
log('Cleaning up the release dir')
to_remove_ext = [
    '.o', '.a', '.pyc', '.lib', '~',
]

to_remove_inf = [
    '.timer.',
]

to_remove_files = [
    'Tupfile',
]

to_remove_dirs = [
    '__pycache__',
]

for root, dirs, files in os.walk(DEST_DIR):
    for f in files:
        if any(f.lower().endswith(ext) for ext in to_remove_ext) or \
           any(inf in f.lower() for inf in to_remove_inf) or \
           f in to_remove_files:
            log(" - Remove file", join(root, f))
            os.unlink(join(root, f))
            continue
    for d in dirs[:]:
        if d in to_remove_dirs:
            dirs.remove(d)
            log(" - Remove directory", join(root, d))
            shutil.rmtree(join(root, d))

if IS_WINDOWS:
    for f in os.listdir(join(DEST_DIR, 'lib')):
        if f.endswith('.dll'):
            p = join(DEST_DIR, 'lib', f)
            log(" - Remove useless dll", p)
            os.unlink(p)

###############################################################################
if not IS_WINDOWS:

    BINARIES = [
        binary.strip() for binary in cmd(
            'find', DEST_DIR, '-type', 'f', '-print'
        ).decode('utf8').split('\n')
        if binary.strip() and is_binary(binary) and 'lib-dynload' not in binary
    ]


    def to_add(dep):
        return not (
            dep.startswith('/usr') or
            dep.startswith('/lib') or
            dep.startswith('/System') or
            dep.startswith(DEST_DIR)
        )

    def fill_dependencies(dependencies, binary_path, indent=1):
        print('-' * indent, binary_path)
        children = dependencies.setdefault(binary_path, set())
        l_dep = list(get_binary_dependencies(binary_path))
        for dependency in l_dep:
            if dependency.startswith('@'):
                continue
            print('-'*indent + '> dep', dependency)
            if not to_add(dependency):
                print('-'*indent + '> IGNORE', dependency)
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

    pprint(SHIPPED_DEPENDENCIES)

    if sys.platform.lower().startswith("darwin"):
        for dep in DEPENDENCIES:
            if dep in SHIPPED_DEPENDENCIES:
                dep = SHIPPED_DEPENDENCIES[dep]
            cmd('chmod', 'u+w', dep)
            reldir = os.path.relpath(DEST_LIB_DIR, start = os.path.dirname(dep))
            if dep.endswith('.so') or dep.endswith('.dylib'):
                cmd('install_name_tool', '-id', '@rpath/' + os.path.basename(dep), dep)
        for dep in DEPENDENCIES:
            print("Working on", dep)
            deps = DEPENDENCIES[dep]
            if dep in SHIPPED_DEPENDENCIES:
                dep = SHIPPED_DEPENDENCIES[dep]
                print("Which is copied at", dep)
            print(dep, "has the follwing deps", deps)
            for subdep in deps:
                buildpath = SHIPPED_DEPENDENCIES[subdep]
                reldir = os.path.relpath(os.path.dirname(buildpath), start = os.path.dirname(dep))
                print(dep, 'subdep', dep, 'at', buildpath)
                cmd(
                    'install_name_tool',
                    '-change',
                    subdep,
                    '@rpath/' + os.path.basename(subdep),
                    dep
                )
                try:
                    cmd('install_name_tool', '-add_rpath', reldir, dep)
                except:
                    print("Couldn't add rpath", reldir, "to", dep)
    else:
        for dep in DEPENDENCIES:
            if dep in SHIPPED_DEPENDENCIES:
                dep = SHIPPED_DEPENDENCIES[dep]
            cmd(
                'patchelf',
                #'--debug',
                '--set-rpath', os.path.join('$' + 'ORIGIN', reldir),
                dep
            )

###############################################################################
log("Stripping release files")
TO_STRIP = []
for root, dirs, files in os.walk(DEST_DIR):
    for f in files:
        path = join(root, f)
        if is_binary(path):
            log(" - Found binary file", path)
            TO_STRIP.append(path)

for bin in TO_STRIP:
    if abspath(bin).startswith(abspath(PYTHON_HOME)):
        log("Ignore", bin)
        continue
    try:
        strip(bin)
    except:
        log("Couldn't strip", bin)

log("Ending release at", datetime.utcnow().ctime())