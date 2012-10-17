# -*- encoding: utf-8 -*-
# The project name
NAME = "8cube"

# The version name (could be "alpha" for example)
VERSION_NAME = "prototype"

from tupcfg import tools, path
from tupcfg import Source
from tupcfg import Target
from tupcfg import Command

class CopyFile(Command):

    @property
    def action(self):
        return "Copying %s to" % self.dependencies[0]

    def command(self, **kw):
        return ['cp', self.dependencies, kw['target']]

class Library:
    def __init__(self, names, include_directories=[], directories=[], shared=False):
        if not isinstance(names, list):
            names = [names]
        self.names = names
        self.directories = directories
        self.include_directories = include_directories
        self.shared = shared


class PythonLibrary(Library):
    def __init__(self, **kw):
        from sysconfig import get_config_var as var
        include_dir = var('INCLUDEPY')
        assert include_dir is not None
        prefix = var('prefix')
        assert prefix is not None
        version = var('py_version_nodot')
        self.ext = var('SO').replace('.', '')
        lib_dirs = []
        for d in ['lib', 'Libs', 'DLLs']:
            if path.exists(prefix, d, 'libpython' + version + '.a') or \
               path.exists(prefix, d, 'python' + version + '.lib') or \
               path.exists(prefix, d, 'python' + version[0] + '.dll') or \
               path.exists(prefix, d, 'libpython' + version + '.so'):
                lib_dirs.append(path.join(prefix, d))
        Library.__init__(self, 'python' + version, [include_dir], lib_dirs, **kw)



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
        library_directories = lib_dirs,
        include_directories = include_dirs + [
            path.absolute(project.root_dir, 'src'),
            path.absolute(project.root_dir, 'src/glew'),
        ],
    )
    status("CXX compiler is", compiler.binary)

    boost_libraries = [Library('boost_python', shared=True)] + list(
        Library('boost_' + s) for s in ['filesystem', 'signals', 'system']
    )
    python_library = PythonLibrary(shared=True)
    python_libraries = [python_library]
    graphic_libraries = list(Library(s, shared=False) for s in ['SDL_image', 'SDL.dll', 'png', 'jpeg']) + [
        Library('freetype', include_directories=['/usr/include/freetype2']),
        Library('z'),
    ]
    if sys.platform == 'win32':
        print("IS WINDOWS")
        graphic_libraries += [
            Library(s) for s in ['opengl32', 'glu32']
        ]
    else:
        print("NOT WINDOWS")
        graphic_libraries += [
            Library(s) for s in ['GL', 'GLU']
        ]

    base_libraries = []
    if sys.platform == 'win32':
        base_libraries += list(Library(name) for name in ['Shlwapi', 'mingw32',])# 'msvcr90'])

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
        compiler.link_library(
            path.splitext(path.basename(binding))[0],
            [binding],
            ext = python_library.ext,
            directory = path.dirname("release/lib/python/cube", binding[9:]),
            libraries=[libcube, libetc] + graphic_libraries + boost_libraries + python_libraries + base_libraries,
        )

    build.add_targets(
        Target(
            'release/lib/python/' + src[4:],
            CopyFile(Source(src))
        ) for src in glob("src/cube/*.py", recursive=True)
    )

    build.add_targets(
        Target(
            'release/lib/python/' + 'cube' + src[4:],
            CopyFile(Source(src))
        ) for src in glob("src/app/*.py", recursive=True)
    )

    build.add_targets(
        Target(
            path.join('release/', src),
            CopyFile(Source(src))
        ) for src in glob("share/games/*.py", recursive=True)
    )

    infinit_cube = compiler.link_executable(
        "8cube",
        glob("src/app/*.cpp", recursive=True),
        directory = "release/bin",
        libraries=[libcube, libetc] + boost_libraries + python_libraries,
    )

    #build.dump(project)
    build.execute(project)
