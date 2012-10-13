# -*- encoding: utf-8 -*-
# The project name
NAME = "8cube"

# The version name (could be "alpha" for example)
VERSION_NAME = "prototype"

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
    def __init__(self, names, include_directories=[], directories=[]):
        if not isinstance(names, list):
            names = [names]
        self.names = names
        self.directories = directories
        self.include_directories = include_directories


from tupcfg import tools

def configure(project, build):
    from tupcfg.tools import glob, status, cleanabspath
    from os.path import join
    status("Configuring project", project.env.NAME, 'in', build.directory, '(%s)' % project.env.BUILD_TYPE)
    from tupcfg.lang.cxx import gcc

    prefixes = project.env.get('PREFIXES', [])
    if isinstance(prefixes, str):
        prefixes = prefixes.split(':')
    project.env.project_set('PREFIXES', prefixes)

    lib_dirs = list(join(p, 'lib') for p in prefixes)
    include_dirs = list(join(p, 'include') for p in prefixes)

    compiler = gcc.Compiler(
        project, build,
        position_independent_code = True,
        standard = 'c++11',
        library_directories = lib_dirs,
        include_directories = include_dirs + [cleanabspath(join(project.root_dir, 'src'))],
    )
    status("CXX compiler is", compiler.binary)

    #compiler = Compiler("g++-4.7", project, build,
    #                    flags=['-fPIC', '-std=c++11', '-x', 'c++',],
    #                    linker_flags=['-L/home/hotgloupi/local/lib'],
    #                    include_directories=[
    #                        '/home/hotgloupi/local/include',
    #                        '/usr/include/python3.2mu',
    #                        '/usr/include/freetype2/',
    #                    ])

    boost_libraries = list(
        Library('boost_' + s) for s in ['python', 'filesystem', 'signals', 'system']
    )
    python_libraries = [
        Library('python3.2mu', include_directories=['/usr/include/python3.2mu'])
    ]
    graphic_libraries = list(Library(s) for s in ['SDL', 'SDL_image', 'GL', 'GLU', 'GLEW']) + [
        Library('freetype', include_directories=['/usr/include/freetype2'])
    ]

    libetc = compiler.link_library(
        'release/lib/libetc.so',
        glob("src/etc/*.cpp", recursive=True),
    )

    libcube = compiler.link_library(
        'release/lib/libcube.so',
        glob("src/cube/*.cpp", recursive=True),
        libraries=[libetc] + graphic_libraries + python_libraries,
    )


    libcube_bindings = []
    for binding in glob("src/cube/*.py++", recursive=True):
        libcube_bindings.append(compiler.link_library(
            join("release/lib/python/cube", binding[9:-5] + '.so'),
            [binding],
            libraries=[libetc, libcube] + boost_libraries + python_libraries,
        ))

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
            join('release/', src),
            CopyFile(Source(src))
        ) for src in glob("share/games/*.py", recursive=True)
    )

    infinit_cube = compiler.link_executable(
        "release/bin/8cube",
        glob("src/app/*.cpp", recursive=True),
        libraries=[libetc, libcube] + boost_libraries + python_libraries,
    )

    #build.dump(project)
    build.execute(project)
