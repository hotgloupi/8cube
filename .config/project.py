# -*- encoding: utf-8 -*-
# The project name
NAME = "8cube"

# The version name (could be "alpha" for example)
VERSION_NAME = "prototype"

from tupcfg import tools, path, platform
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


def configure(project, build):
    import sys
    from tupcfg.tools import glob, status
    from tupcfg import path
    build_type = project.env.get('BUILD_TYPE', 'DEBUG')
    project.env.build_set('BUILD_TYPE', build_type)
    status("Configuring project", project.env.NAME, 'in', build.directory, '(%s)' % build_type)

    prefixes = project.env.get('PREFIXES', [])
    if isinstance(prefixes, str):
        import os
        prefixes = prefixes.split(os.path.pathsep)
    status("Prefixes:", ':'.join(prefixes))
    project.env.project_set('PREFIXES', prefixes)

    include_dirs = project.env.get('INCLUDE_DIRS', [])
    if isinstance(include_dirs, str):
        import os
        include_dirs = include_dirs.split(os.path.pathsep)
    project.env.project_set('INCLUDE_DIRS', include_dirs)


    lib_dirs = list(path.join(p, 'lib') for p in prefixes)
    include_dirs = list(
        dir_ for dir_ in (
            list(path.join(p, 'include') for p in prefixes) + include_dirs
        ) if path.exists(dir_)
    )

    from tupcfg.lang import cxx
    if platform.IS_WINDOWS:
        Compiler = cxx.msvc.Compiler
    else:
        Compiler = cxx.gcc.Compiler

    compiler = Compiler(
        project, build,
        position_independent_code = True,
        standard = 'c++11',
        defines = ['GLM_FORCE_CXX11'],
        library_directories = lib_dirs,
        include_directories = include_dirs + [
            path.absolute(project.root_dir, 'src'),
            path.absolute(project.root_dir, 'src/glew'),
        ],
    )
    status("CXX compiler is", compiler.binary)

    boost = cxxlib.BoostLibrary(
        compiler, components=['system', 'filesystem', 'signals', 'python3']
    )

    python = clib.PythonLibrary(compiler)

    sdl = clib.SDLLibrary(
        compiler,
        components=['image'],
        shared=not platform.IS_WINDOWS
    )

    opengl = clib.OpenGLLibrary(compiler)

    freetype = clib.FreetypeLibrary(compiler)

    graphic_libraries = (
        sdl.libraries +
        opengl.libraries +
        freetype.libraries +
        list(clib.simple(s, compiler) for s in ['png', 'jpeg'])
    )


    base_libraries = []
    if platform.IS_WINDOWS:
        base_libraries += list(clib.simple(name, compiler) for name in ['Shlwapi', 'mingw32',])

    libetc = compiler.link_static_library(
        'libetc',
        glob("src/etc/*.cpp", recursive=True),
        directory  = 'release/lib',
        libraries = base_libraries + boost.libraries
    )

    libglew = compiler.link_static_library(
        'libglew',
        ['src/glew/glew.c'],
        directory = 'release/lib',
        libraries = graphic_libraries,
        defines = ['GLEW_NO_GLU', 'GLEW_BUILD'],
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
