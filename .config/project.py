# -*- encoding: utf-8 -*-
# The project name
NAME = "8cube"

# The version name (could be "alpha" for example)
VERSION_NAME = "prototype"

from tupcfg import tools, path, platform
from tupcfg import generators
from tupcfg import Source
from tupcfg import Target
from tupcfg import Command

from tupcfg.lang.cxx import libraries as cxxlib
from tupcfg.lang.c import libraries as clib

class CopyFile(Command):

    @property
    def action(self):
        return "Copying %s to" % self.dependencies[0]

    def command(self, **kw):
        return ['cp', self.dependencies, kw['target']]


#class PythonCommandGenerator(Generator):
#
#    def __init__(self, **kw):
#        Generator.__
#
#    def apply_rule(self,
#                   action=None,
#                   command=None,
#                   inputs=None,
#                   additional_inputs=None,
#                   outputs=None,
#                   additional_ouputs=None,
#                   target=None):
#        import pipes
#        target_filename = target.shell_string(build=self.build,
#                                              target=target)
#        script = path.join(self.working_directory, target_filename + "_gen.py")
#        with open(script, 'w') as f:
#            write = lambda *args: print(*args, file=f)
#            write("#!python")
#            write("import os, pipes, sys, subprocess")
#            write("print('in', %s)" % repr(self.working_directory))
#            write("os.chdir(%s)" % repr(self.working_directory))
#            write("cmd = [")
#            for p in command:
#                write("\t%s," % repr(p))
#            write("]")
#            write("print(' '.join(map(pipes.quote, cmd)))")
#            write("subprocess.call(cmd, stdout=sys.stdout, stderr=subprocess.STDOUT)")
#        tools.debug("Creating Python command script", script)

def configure(project, build):
    project.generators.append(generators.Makefile)
    import sys
    from tupcfg.tools import glob, status
    from tupcfg import path
    build_type = project.env.get('BUILD_TYPE', 'DEBUG')
    project.env.build_set('BUILD_TYPE', build_type)
    status("Configuring project", project.env.NAME, 'in', build.directory, '(%s)' % build_type)

    from tupcfg.lang import cxx
    if platform.IS_WINDOWS:
        Compiler = cxx.msvc.Compiler
    else:
        Compiler = cxx.gcc.Compiler

    compiler = Compiler(
        project, build,
        position_independent_code = True,
        standard = 'c++11',
        defines = ['GLM_FORCE_CXX11', 'BOOST_ALL_NO_LIB'],
        include_directories = [
            path.absolute(project.root_dir, 'src'),
            path.absolute(project.root_dir, 'src/glew'),
        ],
        static_libstd = False,
        use_build_type_flags = True,
        hidden_visibility = (build_type != 'DEBUG')
    )
    status("CXX compiler is", compiler.binary)

    boost = cxxlib.BoostLibrary(
        compiler,
        components=['system', 'filesystem', 'signals', 'python3'],
        shared=False,
        python3_shared=True,
    )

    python = clib.PythonLibrary(compiler, shared=True)

    sdl = clib.SDLLibrary(
        compiler,
        components=['image'],
        shared=True
    )

    opengl = clib.OpenGLLibrary(compiler)

    freetype = clib.FreetypeLibrary(compiler, shared=False)

    graphic_libraries = (
        sdl.libraries +
        opengl.libraries +
        freetype.libraries
    )
    if not platform.IS_WINDOWS:
        list(clib.simple(s, compiler) for s in ['png', 'jpeg'])

    base_libraries = []
    if platform.IS_WINDOWS:
        base_libraries.extend(
            clib.simple(name, compiler, shared=True) for name in ['Shlwapi',]
        )
    elif platform.IS_MACOSX:
        base_libraries.extend(
            clib.simple(name, compiler, shared=True) for name in ['z', 'bz2',]
        )

    libetc = compiler.link_library(
        'libetc',
        glob("src/etc/*.cpp", recursive=True),
        directory  = 'release/lib',
        libraries = base_libraries + boost.libraries,
        defines = ['ETC_BUILD_DYNAMIC_LIBRARY'],
        shared = True
    )

    libglew = compiler.link_static_library(
        'libglew',
        ['src/glew/glew.c'],
        directory = 'release/lib',
        libraries = graphic_libraries,
        defines = ['GLEW_NO_GLU', 'GLEW_STATIC'],
    )

    graphic_libraries.insert(0, libglew)

    libcube = compiler.link_static_library(
        'libcube',
        glob("src/cube/*.cpp", recursive=True),
        directory  = 'release/lib',
        libraries=[libetc] + graphic_libraries + boost.libraries + python.libraries,
    )


    for binding in glob("src/cube/*.py++", recursive=True):
        compiler.link_dynamic_library(
            path.splitext(path.basename(binding))[0],
            [binding],
            ext = python.ext,
            directory = path.dirname("release/lib/python/cube", binding[9:]),
            libraries=[libcube, libetc] + graphic_libraries + boost.libraries + python.libraries + base_libraries,
        )

    compiler.link_dynamic_library(
        "greenlet",
        ['src/greenlet/greenlet.c'],
        directory = 'release/lib/python',
        ext = python.ext,
        libraries = python.libraries,
    )

    build.add_targets(
        Target(
            'release/lib/python/' + src[4:],
            CopyFile(Source(src))
        ) for src in glob("src/cube/*.py", recursive=True)
    )

    build.add_targets(
        Target(
            'release/lib/python/' + src[4:],
            CopyFile(Source(src))
        ) for src in glob("src/cubeapp/*.py", recursive=True)
    )

    build.add_targets(
        Target(
            path.join('release/share/8cube', src[6:]),
            CopyFile(Source(src))
        ) for src in glob("share/games/*.py", recursive=True)
    )

    infinit_cube = compiler.link_executable(
        "8cube",
        glob("src/cubeapp/*.cpp", recursive=True),
        directory = "release/bin",
        libraries=[libcube, libetc] + graphic_libraries + boost.libraries + python.libraries,
    )

    tests = [
        'simple_window',
    ]

    for test in tests:
        compiler.link_executable(
            test,
            [path.join('tests', test + '.cpp')],
            directory = 'tests',
            libraries = [libcube, libetc] + graphic_libraries + boost.libraries + base_libraries,
        )


