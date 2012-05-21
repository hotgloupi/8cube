# -*- encoding: utf-8 -*-

import sys

# The project name (this is not the version code name).
NAME = 'test'

##
## Note: Every variable can be computed with a command or a function each time
## the configure script is ran. To do so, just set the variable to `None', and
## set another variable named exactly as the first one, but suffixed with '_CMD'.
## For example,
##      >>> VERSION_NAME = None                                 # necessary
##      >>> VERSION_NAME_CMD = "git --describe"                 # retreive with git
##      >>> VERSION_NAME_CMD = ["git", "--describe"]            # safer syntax
##      >>> def myPythonFunction() : return 32                  # simple function
##      ...
##      >>> VERSION_NAME_CMD = _myPythonFunction
##

# The version name (could be "alpha" for example)
VERSION_NAME = 'alpha'
VERSION_NAME_CMD = None

# Availables build types (see build/ files)
BUILD_TYPES = ['Debug', 'Release']
DEFAULT_BUILD_TYPE = 'Debug'

# Tup template file path
#TUP_TEMPLATE_PATH = '.config/templates/Tupfile.templates'
#TUP_TEMPLATE_PATH_CMD = None

# Convenient function to get a portable executable name
def executable(name):
    return name + (sys.platform.startswith('win') and '.exe' or '')

def shared_library_ext():
    return (
        sys.platform.startswith('win') and 'dll' or (
            sys.platform.startswith('mac') and 'dylib' or 'so'
        )
    )

SHARED_LIBRARY_EXT = shared_library_ext()

def static_library(name):
    return name + '.a'

if sys.platform.startswith('win'):
    PYTHON_MODULE_EXT = 'pyd'
else:
    PYTHON_MODULE_EXT = SHARED_LIBRARY_EXT

# Tup template generator (used when TUP_TEMPLATE_PATH is None).
# Can be a python function or a shell command that accept a relative directory
# path as only argument and respectively returns or print out the tup file
# template.
def _getTemplate(dir_):
    with open('.config/templates/Tupfile.templates') as f:
        data = f.read()
    if dir_.startswith('release/lib/python'):

        data += """
LDFLAGS_SHARED = $(BUILD_DIR)/release/lib/libcube.a $(LDFLAGS_SHARED)
: foreach *.o | $(BUILD_DIR)/release/lib/libcube.a |> !link_shared_library |> %B.{PYTHON_MODULE_EXT}

"""
    elif dir_.startswith('src/cube'):
        data = """include_rules

#: foreach {SOURCE_DIR}/*.ipp |> \
#	export f=`$(realpath) %f --relative-to {ROOT_DIR}` && \
#	export cwd=`$(realpath) . --relative-to {ROOT_DIR}` && \
#	cd {ROOT_DIR} && g++ -Isrc -x c++ -std=c++0x -g3 $(CFLAGS) -c ${{f}} -o `$(realpath) ${{cwd}}/%o` \
#	|> %b.o

: foreach {SOURCE_DIR}/*.ipp |> !make_cpp_object |> %b.o

"""
    elif dir_.startswith('src/greenlet'):
        data = "include_rules\n"
        data += ':foreach {SOURCE_DIR}/*.c |> gcc $(CFLAGS) -fPIC -I{PYTHON.include_dir} -c %f -o %o |> %B.o\n'
    return data


TUP_TEMPLATE_GEN = _getTemplate

# tup build specific rules template file path
TUPRULES_TEMPLATE_PATH = '.config/templates/Tuprules.tup.template'
TUPRULES_TEMPLATE_PATH_CMD = None

#
# Define here where source files can be found. The default behavior is to
# recurse over source directories. Set RECURSE_OVER_SOURCE_DIRECTORIES to False
# to change that.
#
SOURCE_DIRECTORIES = [
    'src/app',
    'src/etc',
    'src/cube',
    'src/greenlet',
    ('src/cube', 'release/lib/python'),
    'src/greenlet',
]

# Source directories are treated as tree. If you set this to False, you'll have
# to declare every directory, not just root.
RECURSE_OVER_SOURCE_DIRECTORIES = True

###############################################################################
# Targets is a list that contains high level targets description. Each target
# is described through a dictionary that contains the following keys:
#   - 'input_items' (required): Contains a list input items (see below).
#   - 'additional_inputs' (optional): Contains a list of additional input items
#   - 'command' (required) : The command line to be inserted in the tup rule.
#   - 'output_file' (required): out file name.
#   - 'additional_output_files (optional): a list of additional generated files.
#   - 'output_directory' (optional): where to build out files (defaults to build dir).
#


TARGETS = [
    {
        'input_items': [('src/app', '*.o')],
        'command': '!link_cpp_executable',
        'output_file': executable('my_program'),
        'output_directory': 'release/bin',
    },
    {
        'input_items': [('src/cube', '*.o')],
        'command': '!link_static_library',
        'output_file': 'libcube.a',
        'output_directory': 'release/lib',
    },
    {
        'input_items': ['src/greenlet/greenlet.o'],
        'command': '!link_shared_library',
        'output_file': 'greenlet.{PYTHON_MODULE_EXT}',
        'output_directory': 'release/lib/python',
    }
]

# Same as before
RECURSE_OVER_TARGET_DIRECTORIES = True

###############################################################################
## This section provides examples on how extending project variables (delete or
## comment what's not needed).
##

def setdefault(name, value, globals=globals()):
    return globals.setdefault(name, value)

setdefault("CC", "gcc")
setdefault("CXX", "g++")

setdefault("CCACHE", "ccache")


import tupcfg


LIBRARY_NAMES = [
    'python',
    'GL',
    'GLEW',
    'boost_python3',
    'boost_filesystem',
    'boost_signals',
    'boost_system',
    'SDL',
]

LIBRARIES = dict(
    (libname, tupcfg.find_library(libname, globals()))
    for libname in LIBRARY_NAMES
)

for lib in LIBRARIES.values():
    setdefault(lib.name.upper(), lib)

import sysconfig

CFLAGS = ''
LDFLAGS = ''

LIBRARIES = list(globals()[name.upper()] for name in LIBRARY_NAMES)


include_dirs = set()
static_library_dirs = set()
shared_library_dirs = set()

for library in LIBRARIES:

    try: include_dirs.add(library.include_dir)
    except: pass
    try:
        static_library_dirs.add(library._library_dir('static', quiet=True))
    except: pass
    try:
        shared_library_dirs.add(library._library_dir('shared', quiet=True))
    except: pass

library_dirs = static_library_dirs.union(shared_library_dirs)
library_dirs.remove(None)

if setdefault('BOOST_INCLUDE_DIR', None) is not None:
    include_dirs.add(BOOST_INCLUDE_DIR)

print('#' * 80)
print(include_dirs, library_dirs)
print('#' * 80)

CFLAGS = ' '.join('-I' + d for d in include_dirs)
LDFLAGS = ' '.join('-L' + d for d in library_dirs)

_library_dirs = [
    PYTHON.shared_library_dir,
    BOOST_PYTHON3.shared_library_dir,
    SDL.shared_library_dir,
]

setdefault("SHIPPED_LIBRARIES", [
    'boost_python3',
    'SDL'
])

import os as _os

def _get_libraries_for(lib):
    ext = shared_library_ext()
    for d in _library_dirs:
        for l in _os.listdir(d):
            if lib in l and l.endswith(ext):
                yield d, l

def _get_libraries(libs):
    for lib in libs:
        for dir_, file_ in _get_libraries_for(lib):
            yield dir_, file_

lib_dir = sys.platform.startswith('win') and 'bin' or 'lib'

for dir_, file_ in set(_get_libraries(SHIPPED_LIBRARIES)):
    TARGETS.append({
        'input_items': [],
        'command': 'cp %s .' % cleanjoin(dir_, file_),
        'output_file': file_,
        'output_directory': 'release/' + lib_dir,
    })

