# -*- encoding: utf-8 -*-
# The project name
NAME = "8cube"

# The version name (could be "alpha" for example)
VERSION_NAME = "prototype"

from tupcfg import tools, path
from tupcfg import Source
from tupcfg import Target
from tupcfg import Command
from tupcfg import platform

class CopyFile(Command):

    @property
    def action(self):
        return "Copying %s to" % self.dependencies[0]

    def command(self, **kw):
        return ['cp', self.dependencies, kw['target']]

class Library:
    def __init__(self, names, include_directories=[], directories=[],
                 shared=False, macosx_framework=False):
        if not isinstance(names, list):
            names = [names]
        self.names = names
        self.directories = directories
        self.include_directories = include_directories
        self.shared = shared
        self.macosx_framework = macosx_framework


class PythonLibrary(Library):
    def __init__(self, **kw):
        from sysconfig import get_config_var as var
        include_dir = var('INCLUDEPY')
        assert include_dir is not None
        prefix = var('prefix')
        assert prefix is not None
        version = var('py_version_nodot')
        self.ext = var('SO')[1:]
        lib_dirs = []
        for d in ['lib', 'Libs', 'DLLs']:
            if path.exists(prefix, d, 'libpython' + version + '.a') or \
               path.exists(prefix, d, 'python' + version + '.lib') or \
               path.exists(prefix, d, 'python' + version[0] + '.dll') or \
               path.exists(prefix, d, 'libpython' + version + '.so') or \
               path.exists(prefix, d, 'libpython' + version + '.dylib'):
                lib_dirs.append(path.join(prefix, d))
        if platform.IS_MACOSX:
            lib_dir = var('LIBPL')
            assert path.exists(lib_dir)
            name = 'python' + var('py_version_short')
            assert path.exists(lib_dir, 'lib' + name + '.dylib')
            lib_dirs.append(lib_dir)
        else:
            name = 'python' + (var('LDVERSION') or version)
        print('PYTHON lib dirs:', ', '.join(lib_dirs))
        Library.__init__(self, name, [include_dir], lib_dirs, **kw)

class SDLLibrary(Library):
    def __init__(self, components=[], **kw):
        if platform.IS_WINDOWS:
            names = ['SDL.dll']
        else:
            names = ['SDL']
        for c in components:
            names.append('SDL_' + c)
        if platform.IS_MACOSX:
            names.append('SDLMain') # Fixture from SDL is needed
        Library.__init__(self, names, **kw)

def configure(project, build):
    import sys
    from tupcfg.tools import glob, status
    from tupcfg import path
    status("Configuring project", project.env.NAME, 'in', build.directory, '(%s)' % project.env.BUILD_TYPE)
    from tupcfg.lang.cxx import gcc

    prefixes = project.env.get('PREFIXES', [])
    if isinstance(prefixes, str):
        prefixes = prefixes.split(':')
    project.env.project_set('PREFIXES', prefixes)

    lib_dirs = list(path.join(p, 'lib') for p in prefixes)
    include_dirs = list(path.join(p, 'include') for p in prefixes)

    compiler = gcc.Compiler(
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

    boost_libraries = [Library('boost_python', shared=True)] + list(
        Library('boost_' + s, shared=True) for s in ['filesystem', 'signals', 'system']
    )
    python_library = PythonLibrary(shared=True)
    python_libraries = [python_library]

    graphic_libraries = [
        SDLLibrary(components=['image'], shared=True),
        Library('freetype', include_directories=[
            '/usr/include/freetype2',
            '/opt/local/include/freetype2',
        ], shared=True),
        Library('z', shared=True),
    ] +  list(Library(s, shared=False) for s in ['png', 'jpeg'])

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
            Library(s, shared=True) for s in ['GL', 'GLU']
        ]

    base_libraries = []
    if platform.IS_WINDOWS:
        base_libraries += list(Library(name) for name in ['Shlwapi', 'mingw32',])

    libetc = compiler.link_static_library(
        'libetc',
        glob("src/etc/*.cpp", recursive=True),
        directory  = 'release/lib',
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

    #build.dump(project)
    build.execute(project)
