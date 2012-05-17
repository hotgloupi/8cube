# -*- encoding: utf-8 -*-

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
TUP_TEMPLATE_PATH = '.config/templates/Tupfile.templates'
TUP_TEMPLATE_PATH_CMD = None

# Tup template generator (used when TUP_TEMPLATE_PATH is None).
# Can be a python function or a shell command that accept a relative directory
# path as only argument and respectively returns or print out the tup file
# template.
TUP_TEMPLATE_GEN = None
TUP_TEMPLATE_GEN_CMD = None

# tup build specific rules template file path
TUPRULES_TEMPLATE_PATH = '.config/templates/Tuprules.tup.template'
TUPRULES_TEMPLATE_PATH_CMD = None

#
# Define here where source files can be found. The default behavior is to
# recurse over source directories. Set RECURSE_OVER_SOURCE_DIRECTORIES to False
# to change that.
#
SOURCE_DIRECTORIES = [
    'src',                            # project root dir
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

import sys

# Convenient function to get a portable executable name
def executable(name):
    return name + (sys.platform.startswith('win') and '.exe' or '')

def shared_library_ext():
    return (
        sys.platform.startswith('win') and 'dll' or (
            sys.platform.startswith('mac') and 'dylib' or 'so'
        )
    )

TARGETS = [
    {
        'input_items': [('src', '*.o')],
        'command': '!link_cpp_executable',
        'output_file': executable('my_program'),
        'output_directory': 'release/bin',
    },
]

# Same as before
RECURSE_OVER_TARGET_DIRECTORIES = True

###############################################################################
## This section provides examples on how extending project variables (delete or
## comment what's not needed).
##

def setdefault(name, value, globals=globals()):
    globals.setdefault(name, value)

setdefault("CC", "gcc")
setdefault("CXX", "g++")

setdefault("CCACHE", "ccache")


import sysconfig

setdefault("PYTHON_DIR", cleanpath(sysconfig.get_config_var('prefix')))
setdefault("PYTHON_INCLUDE_DIR", PYTHON_DIR + "/include")
setdefault("PYTHON_LIBRARY_DIR", PYTHON_DIR + "/libs")
setdefault("PYTHON_LIBRARY", 'python32')

setdefault("BOOST_INCLUDE_DIR", None)
setdefault("BOOST_LIBRARY_DIR", None)

setdefault("BOOST_PYTHON_LIBRARY", None)
setdefault("BOOST_FILESYSTEM_LIBRARY", None)
setdefault("BOOST_SYSTEM_LIBRARY", None)


_library_dirs = [
    PYTHON_LIBRARY_DIR,
    BOOST_LIBRARY_DIR,
]

setdefault("SHIPPED_LIBRARIES", [
    'boost_python3',
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

for dir_, file_ in _get_libraries(SHIPPED_LIBRARIES):
    TARGETS.append({
        'input_items': [],
        'command': 'cp %s .' % cleanjoin(dir_, file_),
        'output_file': file_,
        'output_directory': 'release/' + lib_dir,
    })

