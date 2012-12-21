# -*- encoding: utf-8 -*-
# The project name
NAME = "8cube"

# The version name (could be "alpha" for example)
VERSION_NAME = "prototype"

from tupcfg import tools, path, platform
from tupcfg import Source
from tupcfg import Target
from tupcfg import Command
from tupcfg import Library

class CopyFile(Command):

    @property
    def action(self):
        return "Copying %s to" % self.dependencies[0]

    def command(self, **kw):
        return ['cp', self.dependencies, kw['target']]

class PythonLibrary(Library):
    def __init__(self, compiler, **kw):
        from sysconfig import get_config_var as var
        include_dir = var('INCLUDEPY')
        assert include_dir is not None
        prefix = var('prefix')
        assert prefix is not None
        super(PythonLibrary, self).__init__(
            "python" + var('LDVERSION'),
            compiler,
            find_includes = ['Python.h'],
            prefixes = [prefix],
            include_directories = var('INCLUDEPY') and [var('INCLUDEPY')] or [],
            directories = var('LIBPL') and [var('LIBPL')] or [],
            shared = kw.get('shared', True),
        )
        self.ext = var('SO')[1:]
        #version = var('py_version_nodot')
        #lib_dirs = []
        #for d in ['lib', 'Libs', 'DLLs']:
        #    if path.exists(prefix, d, 'libpython' + version + '.a') or \
        #       path.exists(prefix, d, 'python' + version + '.lib') or \
        #       path.exists(prefix, d, 'python' + version[0] + '.dll') or \
        #       path.exists(prefix, d, 'libpython' + version + '.so') or \
        #       path.exists(prefix, d, 'libpython' + version + '.dylib'):
        #        lib_dirs.append(path.join(prefix, d))
        #if platform.IS_MACOSX:
        #    lib_dir = var('LIBPL')
        #    assert path.exists(lib_dir)
        #    name = 'python' + var('py_version_short')
        #    assert path.exists(lib_dir, 'lib' + name + '.dylib')
        #    lib_dirs.append(lib_dir)
        #else:
        #    name = 'python' + (var('LDVERSION') or version)
        #print('PYTHON lib dirs:', ', '.join(lib_dirs))
        #Library.__init__(self, name, [include_dir], lib_dirs, **kw)

class SDLLibrary(Library):
    def __init__(self, compiler, components=[], **kw):
        super(SDLLibrary, self).__init__(
            'SDL',
            compiler,
            find_includes = ['SDL/SDL.h'],
            shared = kw.get('shared', True),
        )
        self.components = list(
            Library('SDL_' + c, compiler, shared = kw.get('shared', True))
            for c in components
        )
        if platform.IS_MACOSX:
            self.components.append(Library('SDLMain')) # Fixture from SDL is needed

    @property
    def libraries(self):
        return [self] + self.components

class BoostLibrary(Library):
    def __init__(self, compiler, components=[], **kw):
        super(BoostLibrary, self).__init__(
            "boost",
            compiler,
            find_includes=['boost/config.hpp'],
            search_binary_files = False,
        )
        self.components = list(
            Library(
                "boost_" + component,
                compiler,
                prefixes = self.prefixes,
                include_directories = self.include_directories,
                directories = self.directories,
                shared = kw.get('%s_shared', kw.get('shared', True)),
            )
            for component in components
        )

    @property
    def libraries(self):
        return self.components

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

    boost_libraries = BoostLibrary(
        compiler, components=['system', 'filesystem', 'signals', 'python3']
    ).libraries

    python_library = PythonLibrary(compiler, shared=True)
    python_libraries = [python_library]

    sdl = SDLLibrary(compiler, components=['image'], shared=not platform.IS_WINDOWS)

    freetype = Library(
        'freetype',
        compiler,
        find_includes = ['ft2build.h', 'freetype/config/ftconfig.h'],
        include_directory_names = ['', 'freetype2'],
        shared=True
    )
    graphic_libraries = sdl.libraries + [
         freetype, Library('z', compiler, shared=True),
    ] + list(Library(s, compiler, shared=False) for s in ['png', 'jpeg'])

    if platform.IS_WINDOWS:
        graphic_libraries += [
            Library(s) for s in ['opengl32', 'glu32']
        ]
    elif platform.IS_MACOSX:
        graphic_libraries.extend(
            Library(name, shared=True, macosx_framework=True) for name in [
                'OpenGL', 'Cocoa', 'CoreFoundation',
            ]
        )
    else:
        graphic_libraries += [
            Library(s, compiler, shared=True) for s in ['GL', 'GLU']
        ]

    base_libraries = []
    if platform.IS_WINDOWS:
        base_libraries += list(Library(name) for name in ['Shlwapi', 'mingw32',])

    libetc = compiler.link_static_library(
        'libetc',
        glob("src/etc/*.cpp", recursive=True),
        directory  = 'release/lib',
        libraries = base_libraries + boost_libraries
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
        libraries=[libetc] + graphic_libraries + boost_libraries + python_libraries,
    )


    for binding in glob("src/cube/*.py++", recursive=True):
        compiler.link_dynamic_library(
            path.splitext(path.basename(binding))[0],
            [binding],
            ext = python_library.ext,
            directory = path.dirname("release/lib/python/cube", binding[9:]),
            libraries=[libcube, libetc] + graphic_libraries + boost_libraries + python_libraries + base_libraries,
        )

    compiler.link_dynamic_library(
        "greenlet",
        ['src/greenlet/greenlet.c'],
        directory = 'release/lib/python',
        ext = python_library.ext,
        libraries = python_libraries,
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
        libraries=[libcube, libetc] + graphic_libraries + boost_libraries + python_libraries,
    )

    tests = [
        'simple_window',
    ]

    for test in tests:
        compiler.link_executable(
            test,
            [path.join('tests', test + '.cpp')],
            directory = 'tests',
            libraries = [libcube, libetc] + graphic_libraries + boost_libraries + base_libraries,
        )
