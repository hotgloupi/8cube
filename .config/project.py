# -*- encoding: utf-8 -*-
# The project name
NAME = "8cube"

# The version name (could be "alpha" for example)
VERSION_NAME = "prototype"

from tupcfg import Command
from tupcfg.command import Shell as ShellCommand
from tupcfg import Dependency
from tupcfg import Source
from tupcfg import Target
from tupcfg import generators
from tupcfg import tools, path, platform

from tupcfg.lang import cxx
from tupcfg.lang import c

class Assimp(Dependency):

    def __init__(self, compiler, source_directory, shared = True):
        super(Assimp, self).__init__(
            "Open Asset Import Library",
            "assimp",
        )
        self.source_directory = source_directory
        self.compiler = compiler
        self.shared = shared
        ext = self.compiler.library_extension(shared)
        self.library_filename = 'libassimp.%s' % ext

    @property
    def targets(self):
        configure_target = Target(
            'assimp/Makefile',
            ShellCommand(
                "Configure Assimp",
                ['cmake', path.absolute(self.source_directory)],
                working_directory = self.build_path(),
            )
        )
        build_target = Target(
            path.join('assimp/code/', self.library_filename),
            ShellCommand(
                "Building Assimp",
                ['make'],
                working_directory = self.build_path(),
                dependencies = [configure_target]
            )
        )
        return [build_target]

    @property
    def libraries(self):
        return [
            cxx.Library(
                'assimp',
                self.compiler,
                shared = self.shared,
                search_binary_files = False,
                include_directories = [
                    path.join(self.source_directory, 'include')
                ],
                directories = [self.build_path('code')],
                files = [self.build_path('code', self.library_filename)],
                save_env_vars = False,
            )
        ]

class FreeType2(Dependency):
    def __init__(self, compiler, source_directory, shared = True):
        super(FreeType2, self).__init__(
            "FreeType2",
            "freetype2",
        )
        self.source_directory = source_directory
        self.compiler = compiler
        self.shared = shared
        ext = self.compiler.library_extension(shared)
        self.library_filename = 'libfreetype.%s' % ext

    @property
    def targets(self):
        configure_target = Target(
            'freetype2/build/Makefile',
            ShellCommand(
                "Configuring Freetype2",
                [
                    path.absolute(self.source_directory, 'configure'),
                    '--prefix', self.build_path('install')
                ],
                working_directory = self.build_path('build')
            )
        )
        build_target = Target(
            'freetype2/build/freetype-config',
            ShellCommand(
                "Building FreeType2",
                ['make'],
                working_directory = self.build_path('build'),
                dependencies = [configure_target]
            )
        )
        install_target = Target(
            path.join('freetype2/install/lib', self.library_filename),
            ShellCommand(
                "Installing FreeType2",
                ['make', 'install'],
                working_directory = self.build_path('build'),
                dependencies = [build_target]
            )
        )
        return [install_target]

    @property
    def libraries(self):
        return [
            cxx.Library(
                'freetype2',
                self.compiler,
                shared = self.shared,
                search_binary_files = False,
                include_directories = [
                    self.build_path('install', 'include', 'freetype2')
                ],
                directories = [self.build_path('install', 'lib')],
                files = [self.build_path('install', 'lib', self.library_filename)],
                save_env_vars = False,
            )
        ]

def configure(project, build):
    import sys
    from tupcfg.tools import glob, rglob, status
    from tupcfg import path
    build_type = project.env.get('BUILD_TYPE', 'DEBUG')
    project.env.build_set('BUILD_TYPE', build_type)
    status("Configuring project", project.env.NAME, '(%s)' % project.env.VERSION_NAME,
           'in', build.directory, '(%s)' % build_type)

    from tupcfg.lang import cxx
    if platform.IS_WINDOWS:
        Compiler = cxx.msvc.Compiler
    else:
        Compiler = cxx.gcc.Compiler

    if project.env.BUILD_TYPE == 'DEBUG':
        defines = [
            'ETC_DEBUG',
            'CUBE_DEBUG',
            'CUBEAPP_DEBUG',
        ]
    elif project.env.BUILD_TYPE == 'RELEASE':
        defines = ['NDEBUG']
    else:
        raise Exception("Unknown build type '%s'" % build_type)

    defines += ['GLM_FORCE_CXX11', 'BOOST_ALL_NO_LIB']

    if platform.IS_LINUX:
        defines += ['CUBE_WITH_GLX']

    compiler = Compiler(
        project, build,
        position_independent_code = True,
        standard = 'c++11',
        defines = defines,
#        library_directories = [
#            '/home/hotgloupi/sandbox/raspberry/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/arm-linux-gnueabihf/libc/lib',
#            '/home/hotgloupi/sandbox/raspberry/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/arm-linux-gnueabihf/libc/lib/arm-linux-gnueabihf',
#            '/home/hotgloupi/sandbox/raspberry/root/usr/lib',
#            '/home/hotgloupi/sandbox/raspberry/root/usr/lib/arm-linux-gnueabihf',
#        ],
        include_directories = [
            path.join(build.directory, 'src'),
#            '/home/hotgloupi/sandbox/raspberry/root/usr/include',
#            '/home/hotgloupi/sandbox/raspberry/root/usr/include/arm-linux-gnueabihf',
            path.absolute(project.root_dir, 'src'),
            path.absolute(project.root_dir, 'src/glew'),
        ],
        stdlib = True, #platform.IS_MACOSX and 'libc++' or True,
        static_libstd = False,
        use_build_type_flags = True,
        hidden_visibility = (build_type != 'DEBUG'),
#        force_architecture = False,
#        additional_link_flags = {
#            'gcc': ['-ldl', '-lpthread', '-lutil', '-lz', '-lX11', '-Xlinker', '-export-dynamic'],
#        }
    )
    status("CXX compiler is", compiler.binary)

    assimp = build.add_dependency(Assimp(compiler, 'deps/assimp'))
    freetype2 = build.add_dependency(FreeType2(compiler, 'deps/freetype2'))

    boost = cxx.libraries.BoostLibrary(
        compiler,
        components=['system', 'filesystem', 'python3', 'thread'],
        preferred_shared=False,
        python3_shared=True,
    )

    python = c.libraries.PythonLibrary(compiler, shared=True)

    sdl = c.libraries.SDLLibrary(
        compiler,
        components=['image'],
        shared=True
    )

    opengl = c.libraries.OpenGLLibrary(compiler)

    graphic_libraries = (
        sdl.libraries +
        opengl.libraries +
        assimp.libraries +
        freetype2.libraries
    )
    if not platform.IS_WINDOWS:
        list(c.libraries.simple(s, compiler) for s in ['png', 'jpeg'])

    base_libraries = []
    if platform.IS_WINDOWS:
        base_libraries.extend(
            c.libraries.simple(name, compiler, shared=True) for name in ['Shlwapi',]
        )
    elif platform.IS_MACOSX:
        base_libraries.extend(
            c.libraries.simple(name, compiler, shared=True) for name in ['z', 'bz2',]
        )

    libglew = compiler.link_static_library(
        'libglew',
        ['src/glew/glew.c'],
        directory = 'release/lib',
        libraries = graphic_libraries,
        defines = ['GLEW_NO_GLU', 'GLEW_STATIC'],
    )

    graphic_libraries.insert(0, libglew)


    stl_pch = compiler.generate_precompiled_header(
        "src/wrappers/stl.hpp",
        force_include = True,
    )

    libetc = compiler.link_library(
        'libetc',
        rglob("src/etc/*.cpp"),
        directory  = 'release/lib',
        libraries = base_libraries + boost.libraries,
        defines = ['ETC_BUILD_DYNAMIC_LIBRARY'],
        shared = True,
        precompiled_headers = [stl_pch]
    )
################### libcube
    boost_python_pch = compiler.generate_precompiled_header(
        "src/wrappers/boost/python.hpp",
        libraries = boost.libraries + python.libraries,
    )

    boost_signals2_pch = compiler.generate_precompiled_header(
        "src/wrappers/boost/signals2.hpp",
        libraries = boost.libraries
    )

    libcube = compiler.link_dynamic_library(
        'libcube',
        rglob("src/cube/*.cpp"),
        directory  = 'release/lib',
        libraries=[libetc] + graphic_libraries + boost.libraries + python.libraries,
        precompiled_headers = [boost_python_pch, boost_signals2_pch, stl_pch],
        defines = ['CUBE_BUILD_DYNAMIC_LIBRARY'],
    )



    for binding in rglob("cube/*.py++", dir='src'):
        t = compiler.link_dynamic_library(
            path.splitext(path.basename(binding))[0],
            [binding],
            ext = python.ext,
            directory = path.dirname("release/lib/python", binding[4:]),
            libraries=[libcube, libetc] + graphic_libraries + boost.libraries + python.libraries + base_libraries,
            precompiled_headers = [boost_python_pch, boost_signals2_pch, stl_pch],
        )

################### libcubeapp

    libcubeapp = compiler.link_static_library(
        'libcubeapp',
        (src for src in rglob("src/cubeapp/*.cpp") if not src.endswith('main.cpp')),
        directory  = 'release/lib',
        libraries = [libcube, libetc] + graphic_libraries + boost.libraries + python.libraries,
        precompiled_headers = [boost_python_pch, boost_signals2_pch, stl_pch],
    )

    for binding in rglob("cubeapp/*.py++", dir='src'):
        compiler.link_dynamic_library(
            path.splitext(path.basename(binding))[0],
            [binding],
            ext = python.ext,
            directory = path.dirname("release/lib/python", binding[4:]),
            libraries=[libcubeapp, libcube, libetc] + graphic_libraries + boost.libraries + python.libraries + base_libraries,
            precompiled_headers = [boost_python_pch, stl_pch],
        )

    infinit_cube = compiler.link_executable(
        "8cube",
        ["src/cubeapp/main.cpp"],
        directory = "release/bin",
        libraries=[libcubeapp, libcube, libetc] + graphic_libraries + boost.libraries + python.libraries,
    )

    for src in rglob("cube/*.py", dir = 'src'):
        build.fs.copy(src, 'release/lib/python/' + src[4:])

    for src in rglob("cubeapp/*.py", dir = 'src'):
        build.fs.copy(src, 'release/lib/python/' + src[4:])

    for src in rglob("games/*.py", dir = 'share'):
        build.fs.copy(src, 'release/share/8cube/' + src[6:])

    tests = [
        'simple_window', 'cube/gl/shader_generator',
    ]

    for test in tests:
        compiler.link_executable(
            test,
            [path.join('tests', test + '.cpp')],
            directory = 'tests',
            libraries = [libcube, libetc] + graphic_libraries + boost.libraries + python.libraries + base_libraries,
            precompiled_headers = [boost_python_pch, boost_signals2_pch, stl_pch],
        )


