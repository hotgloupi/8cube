# -*- encoding: utf-8 -*-
# The project name
NAME = "8cube"

# The version name (could be "alpha" for example)
VERSION_NAME = "prototype"

from tupcfg import Source
from tupcfg import Target
from tupcfg import Command
from tupcfg import Build

class BuildObject(Command):
    action = "Building CXX object"
    def __init__(self, compiler, source):
        if not isinstance(source, Source):
            raise Exception(
                "Cannot build object from '%s' (of type %s)" % (source, type(source))
            )
        self.compiler = compiler
        super(BuildObject, self).__init__(source)

    def command(self, **kw):
        from os.path import join
        from tupcfg.tools import cleanabspath
        return (
            [self.compiler.binary] +
            self.compiler.flags +
            list(('-I' + i) for i in self.compiler.include_directories),
            '-I', cleanabspath(join(kw['build'].root_directory, 'src')),
            '-fPIC',
            '-c', self.dependencies[0],
            '-o', kw['target'],
        )

class LinkCommand(Command):
    class LinkFlag:
        def __init__(self, lib, method):
            self.lib = lib
            self.method = method

        def shell_string(self, **kw):
            return self.method(self.lib, **kw)

    def __init__(self, compiler, objects, flags=[], link_libraries=[]):
        self.compiler = compiler
        self.flags = flags
        self.linker_flags = []
        from os.path import dirname, abspath
        closure = set()
        def add_rpath(lib, **kw):
            p = '-Wl,-rpath=\\$ORIGIN/' + dirname(lib.relpath(kw['target'], **kw))
            if p in closure:
                return ''
            else:
                closure.add(p)
                return p

        for lib in link_libraries:
            if isinstance(lib, Target):
                self.linker_flags.append(self.LinkFlag(lib, add_rpath))
                self.linker_flags.append(lib)
            else:
                if lib.directories:
                    self.linker_flags.extend('-L' + dir_ for dir_ in lib.directories)
                self.linker_flags.extend(('-l' + name) for name in lib.names)
        self.link_libraries = link_libraries
        super(LinkCommand, self).__init__(objects)


class LinkExecutable(LinkCommand):
    action = "Linking executable"


    def command(self, **kw):
        return (
            [self.compiler.binary] +
            self.flags +
            self.dependencies +
            ['-o', kw['target']] +
            self.compiler.linker_flags +
            self.linker_flags
        )

class LinkLibrary(LinkCommand):
    action = "Linking library"

    def command(self, **kw):
        from os.path import basename
        def set_soname(lib, **kw):
            return '-Wl,-soname=' + basename(lib.name)

        return (
            [self.compiler.binary] +
            self.compiler.linker_flags +
            self.flags +
            self.dependencies +
            ['-shared', '-o', kw['target']] +
            [self.LinkFlag(kw['target'], set_soname)] +
            self.compiler.linker_flags +
            self.linker_flags
        )

class CopyFile(Command):

    @property
    def action(self):
        return "Copying %s to" % self.dependencies[0]

    def command(self, **kw):
        return ['cp', self.dependencies, kw['target']]

class Library:
    def __init__(self, names, directories=[]):
        if not isinstance(names, list):
            names = [names]
        self.names = names
        self.directories = directories


from tupcfg import tools

class Compiler:

    def __init__(self, name, project, build,
                 flags=[],
                 linker_flags=[],
                 include_directories=[]):
        self.project = project
        self.build = build
        self.linker_flags = linker_flags
        self.binary = tools.find_binary(name)
        self.flags = flags
        self.include_directories = include_directories

    def link_library(self, name, sources, **kw):
        sources_ = (Source(src) for src in sources)
        objects = (Target(src.filename + '.o', BuildObject(self, src)) for src in sources_)
        return self.build.add_target(Target(name, LinkLibrary(self, list(objects), **kw)))


    def link_executable(self, name, sources, **kw):
        sources_ = (Source(src) for src in sources)
        objects = (Target(src.filename + '.o', BuildObject(self, src)) for src in sources_)
        return self.build.add_target(Target(name, LinkExecutable(self, list(objects), **kw)))



def configure(project, build):
    from tupcfg.tools import glob, status
    from os.path import join
    status("Configuring project", project.env.NAME, 'in', build.directory, '(%s)' % project.env.BUILD_TYPE)

    compiler = Compiler("g++-4.7", project, build,
                        flags=['-fPIC', '-std=c++11', '-x', 'c++',],
                        linker_flags=['-L/home/hotgloupi/local/lib'],
                        include_directories=[
                            '/home/hotgloupi/local/include',
                            '/usr/include/python3.2mu',
                            '/usr/include/freetype2/',
                        ])

    boost_libraries = list(
        Library('boost_' + s) for s in ['python', 'filesystem', 'signals', 'system']
    )
    python_libraries = [
        Library('python3.2mu'),
    ]
    graphic_libraries = list(Library(s) for s in ['SDL', 'SDL_image', 'GL', 'GLU', 'GLEW', 'freetype'])

    libetc = compiler.link_library(
        'release/lib/libetc.so',
        glob("src/etc/*.cpp", recursive=True),
    )

    libcube = compiler.link_library(
        'release/lib/libcube.so',
        glob("src/cube/*.cpp", recursive=True),
        link_libraries=[libetc] + graphic_libraries,
    )


    libcube_bindings = []
    for binding in glob("src/cube/*.py++", recursive=True):
        libcube_bindings.append(compiler.link_library(
            join("release/lib/python/cube", binding[9:-5] + '.so'),
            [binding],
            link_libraries=[libetc, libcube] + boost_libraries + python_libraries,
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
        link_libraries=[libetc, libcube] + boost_libraries + python_libraries,
    )

    #build.dump(project)
    build.execute(project)
