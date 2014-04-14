# -*- encoding: utf-8 -*-
# The project name
NAME = "8cube"

# The version name (could be "alpha" for example)
VERSION_NAME = "prototype"

from configure import Command
from configure.command import Command
from configure import Dependency
from configure import Source
from configure import Target
from configure import generators
from configure import tools, path, platform

from configure.lang import cxx
from configure.lang import c

from configure.tools import glob, rglob, status
from configure import path

from configure.dependency.cmake import CMakeDependency

class Assimp(CMakeDependency):
    def __init__(self,
                 build,
                 compiler,
                 source_directory,
                 boost = None,
                 shared = True,
                 c_compiler = None):
        super().__init__(
            build,
            "Assimp",
            compiler = compiler,
            source_directory = source_directory,
            libraries = [
                {
                    'prefix': compiler.name != 'msvc' and 'lib' or '',
                    'name': 'assimp',
                    'shared': shared,
                }
            ],
            configure_variables = [
                ('CMAKE_C_COMPILER', c_compiler.binary),
                ('Boost_DEBUG', True),
                ('Boost_DETAILED_FAILURE_MSG', True),
                ('Boost_NO_SYSTEM_PATHS', True),
                ('Boost_NO_CMAKE', True,),
                ('Boost_ADDITIONAL_VERSIONS', '1.55'),
                ('ASSIMP_BUILD_ASSIMP_TOOLS', False),
                ('ASSIMP_BUILD_STATIC_LIB', not shared),
                ('ASSIMP_BUILD_SAMPLES', False),
                ('ASSIMP_BUILD_TESTS', False),
                ('ASSIMP_NO_EXPORT', True),
                ('ASSIMP_DEBUG_POSTFIX', ''),
            ],
            configure_env = {
                'BOOST_ROOT': boost.root_directory,
                'BOOST_INCLUDEDIR': boost.include_directory,
                'BOOST_LIBRARYDIR': boost.library_directory,
            }
        )
        if not shared and platform.IS_WINDOWS:
            dir = self.absolute_build_path('build/contrib/zlib')
            if compiler.name == 'msvc':
                dir = path.join(dir, self.build_type)
                files = [path.join(dir, 'zlib.lib')]
            else:
                files = [path.join(dir, 'libzlib.a')]
            self.libraries.append(
                compiler.Library(
                    'libz',
                    self.compiler,
                    shared = False,
                    search_binary_files = False,
                    include_directories = [],
                    directories = [dir],
                    files = files,
                    save_env_vars = False,
                )
            )

class GLM(Dependency):
    def __init__(self, build, compiler, source_directory):
        super().__init__(
            build,
            "GLM",
            source_directory = source_directory
        )
        self.compiler = compiler

    @property
    def targets(self):
        return []

    @property
    def libraries(self):
        return [
            cxx.Library(
                'GLM',
                self.compiler,
                search_binary_files = False,
                include_directories = [self.absolute_source_path()],
                save_env_vars = False,
            )
        ]

def main(project, build):
    build_type = project.env.get('BUILD_TYPE', 'DEBUG')
    project.env.build_set('BUILD_TYPE', build_type)
    status("Configuring project", project.env.NAME, '(%s)' % project.env.VERSION_NAME,
           'in', build.directory, '(%s)' % build_type)

    from configure.lang import cxx

    if project.env.BUILD_TYPE == 'DEBUG':
        defines = [
            'ETC_DEBUG',
            'CUBE_DEBUG',
            'CUBEAPP_DEBUG',
        ]
    elif project.env.BUILD_TYPE == 'RELEASE':
        defines = ['NDEBUG']
        if platform.IS_WINDOWS:
            defines.append(('_SCL_SECURE', '0'))
    else:
        raise Exception("Unknown build type '%s'" % build_type)

    defines += ['BOOST_ALL_NO_LIB']

    if platform.IS_WINDOWS:
        defines.extend([
            'NOMINMAX'
        #    ('_WIN32_WINNT', '0x0600'),
        #    ('WINVER', '0x0600'),
        ])
    #if platform.IS_LINUX:
    #    defines += ['CUBE_WITH_GLX']

    library_directories = [
        #'/home/hotgloupi/sandbox/raspberry/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/arm-linux-gnueabihf/libc/lib',
        #'/home/hotgloupi/sandbox/raspberry/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/arm-linux-gnueabihf/libc/lib/arm-linux-gnueabihf',
        #'/home/hotgloupi/sandbox/raspberry/root/usr/lib',
        #'/home/hotgloupi/sandbox/raspberry/root/usr/lib/arm-linux-gnueabihf',
    ]
    include_directories = [
        path.join(build.directory, 'src'),
#        '/home/hotgloupi/sandbox/raspberry/root/usr/include',
#        '/home/hotgloupi/sandbox/raspberry/root/usr/include/arm-linux-gnueabihf',
        path.absolute(project.root_dir, 'src'),
        path.absolute(project.root_dir, 'src/glew'),
    ]
    compiler = cxx.find_compiler(
        project, build,
        position_independent_code = True,
        standard = 'c++11',
        defines = defines,
        library_directories = library_directories,
        include_directories = include_directories,
        static_libstd = False,
        use_build_type_flags = True,
        hidden_visibility = (build_type != 'DEBUG'),
        #force_architecture = True,
        #target_architecture = '32bit',
        forbidden_warnings = ['return-type',]
#        additional_link_flags = {
#            'gcc': ['-ldl', '-lpthread', '-lutil', '-lz', '-lX11', '-Xlinker', '-export-dynamic'],
#        }
    )

    c_compiler = c.find_compiler(project, build, force_architecture = False)

    status("Using %s as C++ compiler" % compiler)
    status("Using %s as C compiler" % c_compiler)

    freetype2 = build.add_dependency(
        c.libraries.FreetypeDependency,
        c_compiler,
        'deps/freetype2'
    )

    curl = build.add_dependency(
        c.libraries.CURLDependency,
        c_compiler,
        "deps/curl-7.35.0",
        shared = False, #platform.IS_WINDOWS,
        with_ldap = False,
        with_ldaps = False,
        with_ftp = False,
        with_tftp = False,
        with_telnet = False,
        with_dict = False,
        with_file = False,
        with_cookies = True,
    )

    if platform.IS_WINDOWS:
        python = c.libraries.PythonLibrary(c_compiler, shared = True)
    else:
        python = build.add_dependency(
            c.libraries.PythonDependency,
            c_compiler,
            'deps/Python-v3.4.0',
            shared = platform.IS_LINUX,
            version = (3, 4),
            pymalloc = False,
            with_valgrind_support = False,
        )

    boost = build.add_dependency(
        cxx.libraries.BoostDependency,
        compiler,
        'deps/Boost_1_55_0',
        version = (1, 55),
        python = python,
        components = [
            'format',
            'timer',
            'system',
            'filesystem',
            'python',
            'signals2',
            'thread',
            'coroutine',
            'context',
        ],
        #preferred_shared = False,
        #        python3_shared = True,
    )

    glm = build.add_dependency(GLM, compiler, "deps/glm")

    # XXX needed ?
    compiler.include_directories.extend(tools.unique(
        sum((l.include_directories for l in boost.libraries),  [])
    ))

    assimp = build.add_dependency(
        Assimp,
        compiler,
        'deps/assimp',
        boost = boost,
        c_compiler = c_compiler,
        shared = False
    )

    sdl = build.add_dependency(
        c.libraries.SDLDependency,
        c_compiler,
        'deps/SDL',
        shared = True, #platform.IS_WINDOWS,
        audio = False,
    )
    sdl_image = build.add_dependency(
        c.libraries.SDLImageDependency, c_compiler, 'deps/SDL_image',
        sdl = sdl,
        shared = False
    )
    #sdl = c.libraries.SDLLibrary(
    #    compiler,
    #    components=['image'],
    #    shared=True
    #)

    opengl = c.libraries.OpenGLLibrary(
        compiler,
        system = True,
        shared = compiler.name != 'msvc'
    )

    graphic_libraries = (
        sdl.libraries +
        sdl_image.libraries +
        opengl.libraries +
        #assimp.libraries +
        freetype2.libraries +
        glm.libraries
    )
    #if not platform.IS_WINDOWS:
    #    list(c.libraries.simple(s, compiler) for s in ['png', 'jpeg'])

    base_libraries = []
    if platform.IS_WINDOWS:
        base_libraries.extend(
            c.libraries.simple(name, compiler, system = True) for name in [
                'Shlwapi',
                'ws2_32',
                'Gdi32',
                'mswsock',
                'Dbghelp',
                'winmm',
                'version',
                'imm32',
                'Shell32',
                'User32',
                'Ole32',
                'OleAut32',
                'Advapi32',
                'Kernel32',
        #'msvcrt', 'libcmt'
            ]
        )
    else: # OSX and Linux
        base_libraries.extend(
            c.libraries.simple(name, compiler, system = True) for name in ['z', 'bz2',]
        )
        if platform.IS_LINUX:
            pass
            # SDL audio disabled
            #base_libraries.extend(
            #    c.libraries.simple(name, compiler, system = True) for name in ['audio',]
            #)
        elif platform.IS_MACOSX:
            base_libraries.extend(
                c.libraries.simple(name, compiler, macosx_framework = True)
                for name in [
                    'ForceFeedback', 'IOKit', 'Cocoa', 'Carbon',
                    # 'AudioUnit', 'CoreAudio', 'AudioToolbox', # SDL Audio disabled
                ]
            )

    libglew = compiler.link_static_library(
        'libglew',
        ['src/glew/glew.c'],
        directory = 'release/lib',
        libraries = opengl.libraries,
        defines = ['GLEW_NO_GLU', 'GLEW_STATIC'],
    )

    graphic_libraries.insert(0, libglew)


    precompiled_headers = []

    with_pch = False
    if with_pch:
        precompiled_headers.append(
            compiler.generate_precompiled_header(
                "src/wrappers/stl.hpp",
                force_include = True,
            )
        )

    base_libraries.extend([
        boost.component_library('filesystem'),
        boost.component_library('system'),
        boost.component_library('thread'),
        boost.component_library('coroutine'),
        boost.component_library('context'),
    ])
    libetc = compiler.link_library(
        'libetc',
        rglob("src/etc/*.cpp"),
        directory  = 'release/lib',
        libraries = base_libraries + curl.libraries,
        defines = ['ETC_BUILD_DYNAMIC_LIBRARY', ('CURL_STATICLIB', 1)],
        shared = True,
        precompiled_headers = precompiled_headers,
    )
    libetc_static = compiler.link_library(
        'libetc-static',
        rglob("src/etc/*.cpp"),
        directory  = 'release/lib',
        object_directory = 'etc-static',
        libraries = base_libraries + curl.libraries,
        defines = ['ETC_BUILD_STATIC_LIBRARY', ('CURL_STATICLIB', 1)],
        shared = False,
        position_independent_code = True,
        static_libstd = not platform.IS_MACOSX,
        precompiled_headers = precompiled_headers,
    )

################### libcube
    if with_pch:
        precompiled_headers.extend([
            compiler.generate_precompiled_header(
                "src/wrappers/boost/signals2.hpp",
                libraries = boost.libraries
            ),
        ])

    libcube_libs = base_libraries + [libetc] + assimp.libraries + graphic_libraries

    libcube = compiler.link_dynamic_library(
        'libcube',
        rglob("src/cube/*.cpp"),
        directory  = 'release/lib',
        libraries = libcube_libs,
        precompiled_headers = precompiled_headers,
        defines = ['CUBE_BUILD_DYNAMIC_LIBRARY'],
    )


    if with_pch:
        precompiled_headers.extend([
            compiler.generate_precompiled_header(
                "src/wrappers/boost/python.hpp",
                libraries = boost.libraries + python.libraries,
            ),
        ])

    python_module_libraries = boost.libraries + python.libraries
    if platform.IS_WINDOWS:
        python_module_libraries.extend([libcube, libetc])
    for binding in rglob("cube/*.py++", dir='src'):
        t = compiler.link_dynamic_library(
            path.splitext(path.basename(binding))[0],
            [binding],
            ext = python.ext,
            directory = path.dirname("release/lib/python", binding[4:]),
            libraries = python_module_libraries + base_libraries,
            include_directories = glm.libraries[0].include_directories,
            precompiled_headers = precompiled_headers,
            allow_unresolved_symbols = True,
        )

################### libcubeapp

    libcubeapp = compiler.link_dynamic_library(
        'libcubeapp',
        (src for src in rglob("src/cubeapp/*.cpp") if not src.endswith('main.cpp')),
        directory  = 'release/lib',
        libraries = [libetc, libcube] + boost.libraries + python.libraries + glm.libraries,
        precompiled_headers = precompiled_headers,
        defines = ['CUBEAPP_BUILD_DYNAMIC_LIBRARY'],
    )

    for binding in rglob("cubeapp/*.py++", dir='src'):
        compiler.link_dynamic_library(
            path.splitext(path.basename(binding))[0],
            [binding],
            ext = python.ext,
            directory = path.dirname("release/lib/python", binding[4:]),
            libraries=[libcubeapp, libcube, libetc] + graphic_libraries + boost.libraries + python.libraries + base_libraries,
            precompiled_headers = precompiled_headers,
        )


    infinit_cube_libraries = [
        libcubeapp,
        libcube,
        libetc,
    ] + graphic_libraries + boost.libraries + python.libraries + base_libraries

    cube_exe = compiler.link_executable(
        "8cube",
        ["src/cubeapp/main.cpp"],
        directory = "release/bin",
        libraries = infinit_cube_libraries,
    )



    if platform.IS_WINDOWS:
        for lib in infinit_cube_libraries + assimp.libraries:
            if not isinstance(lib, Target):
                #print("lib %s" % lib.files, lib.shared, lib.system)
                if lib.shared and not lib.system:
                    for f in lib.files:
                        #print("copy", f)
                        build.fs.copy(f, dest_dir = 'release/bin')
            else:
                if lib.shared:
                    #print("copy %s" % lib.path)
                    build.fs.copy(lib, dest_dir = 'release/bin')


    exts = ['py', 'bmp', 'ttf']

    for ext in exts:
        for src in rglob("cube/*.%s" % ext, dir = 'src'):
            build.fs.copy(src, 'release/lib/python/' + src[4:])
        for src in rglob("cubeapp/*.%s" % ext, dir = 'src'):
            build.fs.copy(src, 'release/lib/python/' + src[4:])


    #import os
    #for game in os.listdir('share/games'):
    #    game_path = os.path.join('share/games', game)
    #    if not os.path.isdir(game_path):
    #        continue
    #    for src in rglob(path.join(game_path, '*.py')):
    #        build.fs.copy(src, src[6:])


    # Default game launcher
    #
    # By default, it will launch the default game (named "default"). The
    # environment variable "GAME_ID" is used takes precedence over the default
    # game
    #
    compiler.link_executable(
        "launch",
        list(rglob("*.cpp", dir = "src/launch")),
        directory = 'release/bin',
        defines = [('GAME_ID', "default"), 'ETC_STATIC_LIBRARY'],
        static_libstd = not platform.IS_MACOSX and not platform.IS_WINDOWS,
        libraries =
            [libetc_static] +
            base_libraries +
            (
                not platform.IS_WINDOWS and
                [c.libraries.simple('pthread', compiler, system = True)] or
                []
            ) +
            curl.libraries
    )

    tests = [
        'simple_window', 'cube/gl/shader_generator',
    ]

    #for test in tests:
    #    compiler.link_executable(
    #        test,
    #        [path.join('tests', test + '.cpp')],
    #        directory = 'tests',
    #        libraries = [libcube, libetc] + graphic_libraries + boost.libraries + python.libraries + base_libraries,
    #        precompiled_headers = [boost_python_pch, boost_signals2_pch, stl_pch],
    #    )


