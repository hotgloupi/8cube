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
        data += ':foreach {SOURCE_DIR}/*.c |>!make_c_object |>\n'
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
    #{
    #    'input_items': ['src/greenlet/greenlet.o'],
    #    'command': '!link_shared_library',
    #    'output_file': 'greenlet.{PYTHON_MODULE_EXT}',
    #    'output_directory': 'release/lib/python',
    #}
]

# Same as before
RECURSE_OVER_TARGET_DIRECTORIES = True

###############################################################################
## This section provides examples on how extending project variables (delete or
## comment what's not needed).
##

def setdefault(name, value, globals=globals()):
    return globals.setdefault(name, value)

setdefault('PREFIX', '/usr')

import distutils.ccompiler
import distutils.sysconfig

py_libname = None
try:
    py_ldflags = distutils.sysconfig.get_config_var('BLDLIBRARY').split(' ')
    for f in py_ldflags:
        if f.startswith('-l'):
            py_libname = f[2:]
            break
except:
    py_libname = 'python3'

INCLUDE_DIRS = [
    '$(SOURCE_DIR)',
    distutils.sysconfig.get_python_inc(),
    cleanjoin(PREFIX, 'include'),
]

LIBRARY_DIRS = [
    cleanjoin(PREFIX, 'lib'),
    '/usr/lib/i386-linux-gnu', # for libGL.so
]

RUNTIME_LIBRARY_DIRS = LIBRARY_DIRS + [
]

STATIC_LIBRARY_NAMES = [
]

RUNTIME_LIBRARY_NAMES = [
    'GLU',
    'GLEW',
    'SDL',
    'SDL_image',
    'boost_system',
    'boost_filesystem',
    py_libname,
    'boost_signals',
    #'boost_python-py32',
    'boost_python3',
    'GL',
    'asound',
    'dl',
    'pulse-simple',
    'pulse',
    'caca',
    'm',
    'Xext',
    'X11',
    'pthread',
    'xcb',
    'Xau',
    'Xdmcp',
]

LIBRARY_NAMES = STATIC_LIBRARY_NAMES + RUNTIME_LIBRARY_NAMES
SHIPPED_LIBRARY_NAMES = [
    #py_libname,
]
gcc = distutils.ccompiler.new_compiler()

gcc.set_include_dirs(INCLUDE_DIRS)
gcc.set_library_dirs(LIBRARY_DIRS)
gcc.set_runtime_library_dirs(RUNTIME_LIBRARY_DIRS)
gcc.set_libraries(LIBRARY_NAMES)

gcc.language_map['.ipp'] = 'c++'
gcc.src_extensions.append('.ipp')

class CommandCatcher:
    def __init__(self, compiler, defaults={}):
        self.compiler = compiler
        self.compiler.spawn = self
        self.defaults = defaults

    def __call__(self, cmd):
        #print("GOT", cmd)
        self._cmd = cmd

    def cmd(self, method, in_files, out_file=None, **kwargs):
        if method not in []:
            for k, v in self.defaults.items():
                kwargs.setdefault(k, v)
        if not isinstance(in_files, list):
            in_files = [in_files]
        extra_objects = kwargs.pop('extra_objects', [])

        method = getattr(self.compiler, method)
        if out_file:
            out = method(in_files, out_file, **kwargs)
        else:
            out = method(in_files, **kwargs)
        if out is None:
            out = out_file
        if isinstance(out, list):
            assert(len(out) == 1)
            out = out[0]
        #print("   -->", out)
        res = []
        static_libs = []
        dynamic_libs = []
        objects = []
        outflag = False
        for p in self._cmd:
            if p in in_files:
                p = '%f'
            if out in p:
                p = '%o'

            if p.startswith('-Wl') or p.startswith('-L'):
                dynamic_libs.append(p)
            elif p.startswith('-l'):
                if p[2:] in STATIC_LIBRARY_NAMES:
                    static_libs.append(p)
                else:
                    dynamic_libs.append(p)
            elif p == '%f':
                objects.append(p)
            else:
                if p == '-o':
                    outflag = True
                else:
                    outflag = False
                res.append(p)
        for i, p in enumerate(res):
            if p == '-o':
                break
        objects = extra_objects + objects + extra_objects
        if len(static_libs):
            static_libs = ['-Wl,-Bstatic'] + static_libs
        if len(dynamic_libs):
            dynamic_libs = ['-Wl,-Bdynamic', '-Wl,-R,\$ORIGIN/../lib'] + dynamic_libs
        print(objects, static_libs, dynamic_libs)
        res = res[:i] + objects + static_libs + dynamic_libs + res[i:]
        return ' '.join(res)


catcher = CommandCatcher(gcc, {
    'extra_preargs': '-std=c++0x -ggdb3 -Wall -Wextra'.split(' '),
})

MAKE_C_OBJECT = catcher.cmd(
    'compile', ['SRC_MARKER.c'],
    extra_preargs = '-ggdb3 -Wall -Wextra -fPIC'.split(' ')
)

for k, v in {
    'compiler'     : ["g++"],
    'compiler_so'  : ["g++"],
    'compiler_cxx' : ["g++"],
    'linker_so'    : ["g++", "-shared"],
    'linker_exe'   : ["g++"]
}.items():
    gcc.set_executable(k, v)

MAKE_CPP_OBJECT = catcher.cmd(
    'compile', ['SRC_MARKER.cpp'],
    extra_preargs = '-x c++ -std=c++0x -ggdb3 -Wall -Wextra -fPIC'.split(' '),
)


LINK_CPP_EXECUTABLE = catcher.cmd(
    'link_executable', ['OBJECT_MARKER.o'],
    'OUTPUT_MARKER',
)

LINK_CPP_SHARED_LIBRARY = catcher.cmd(
    'link_shared_lib', ['OBJECT_MARKER.o'], 'OUTPUT_MARKER',
    extra_objects = ['$(LDFLAGS_SHARED)',],
)

print(MAKE_CPP_OBJECT)
print(LINK_CPP_EXECUTABLE)
print(LINK_CPP_SHARED_LIBRARY)
import os
lib_dir = sys.platform.startswith('win') and 'bin' or 'lib'
for lib in SHIPPED_LIBRARY_NAMES:
    path = gcc.find_library_file(LIBRARY_DIRS + RUNTIME_LIBRARY_DIRS, lib)
    if path is None:
        print("WARNING: Cannot ship library file for '%s'" % lib)
        continue
    fname = os.path.basename(path)

    TARGETS.append({
        'input_items': [],
        'command': 'cp -Lp %s . && chmod +x %s' % (path, fname),
        'output_file': fname,
        'output_directory': 'release/' + lib_dir,
    })

setdefault("CC", "gcc")
setdefault("CXX", "g++")
#
#setdefault("CCACHE", "ccache")
#
#
#import tupcfg
#
#
#LIBRARY_NAMES = [
#    'python',
#    'GL',
#    'GLEW',
#    'boost_python3',
#    'boost_filesystem',
#    'boost_signals',
#    'boost_system',
#    'SDL',
#]
#
#LIBRARIES = dict(
#    (libname, tupcfg.find_library(libname, globals()))
#    for libname in LIBRARY_NAMES
#)
#
#for lib in LIBRARIES.values():
#    setdefault(lib.name.upper(), lib)
#
#import sysconfig
#
#CFLAGS = ''
#LDFLAGS = ''
#
#LIBRARIES = list(globals()[name.upper()] for name in LIBRARY_NAMES)
#
#for l in LIBRARIES:
#    for attr in [
#        'include_dir',
#        'shared_library',
#        'shared_library_filename',
#        'shared_library_filepath',
#    ]:
#        print(l.name.upper() + '.' + attr, '=',  getattr(l, attr))
#
#
#include_dirs = set()
#static_library_dirs = set()
#shared_library_dirs = set()
#
#for library in LIBRARIES:
#
#    try: include_dirs.add(library.include_dir)
#    except: pass
#    try:
#        static_library_dirs.add(library._library_dir('static', quiet=True))
#    except: pass
#    try:
#        shared_library_dirs.add(library._library_dir('shared', quiet=True))
#    except: pass
#
#library_dirs = static_library_dirs.union(shared_library_dirs)
#library_dirs.remove(None)
#
#if setdefault('BOOST_INCLUDE_DIR', None) is not None:
#    include_dirs.add(BOOST_INCLUDE_DIR)
#
#print('#' * 80)
#print(include_dirs, library_dirs)
#print('#' * 80)
#
#CFLAGS = ' '.join('-I' + d for d in include_dirs)
#LDFLAGS = ' '.join('-L' + d for d in library_dirs)
#
#setdefault("SHIPPED_LIBRARIES", [
#    l for l in LIBRARIES if l.shared_library
#])
#
#import os
#
#def _get_libraries(libs):
#    for lib in libs:
#        if lib.shared_library:
#            p = lib.shared_library_filepath
#            yield os.path.split(p)
#
#
