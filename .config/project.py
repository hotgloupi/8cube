# -*- encoding: utf-8 -*-
# The project name
NAME = "8cube"

# The version name (could be "alpha" for example)
VERSION_NAME = "prototype"

from tupcfg import Command
from tupcfg import Dependency
from tupcfg import Source
from tupcfg import Target
from tupcfg import generators
from tupcfg import tools, path, platform

from tupcfg.lang import cxx
from tupcfg.lang import c

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
            path.absolute(project.root_dir, 'src/freetype-2.4.11/include'),
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

    freetype = compiler.link_static_library(
        'libfreetype2',
        (
            f for f in glob('src/freetype-2.4.11/src/*.c', recursive=True)
            if not any(
                p in f for p in [
                    "afpic.c",
                    "afangles.c",
                    "afglobal.c",
                    "afhints.c",
                    "afdummy.c",
                    "aflatin.c",
                    "aflatin2.c",
                    "afcjk.c",
                    "afindic.c",
                    "afloader.c",
                    "afmodule.c",
                    "afwarp.c",
                    "ftpic.c",
                    "basepic.c",
                    "ftadvanc.c",
                    "ftcalc.c",
                    "ftdbgmem.c",
                    "ftgloadr.c",
                    "ftobjs.c",
                    "ftoutln.c",
                    "ftrfork.c",
                    "ftsnames.c",
                    "ftstream.c",
                    "fttrigon.c",
                    "ftutil.c",
                    "ftmac.c",
                    "bdflib.c",
                    "bdfdrivr.c",
                    "ftcmru.c",
                    "ftcmanag.c",
                    "ftccache.c",
                    "ftccmap.c",
                    "ftcglyph.c",
                    "ftcimage.c",
                    "ftcsbits.c",
                    "ftcbasic.c",
                    "cffpic.c",
                    "cffdrivr.c",
                    "cffparse.c",
                    "cffload.c",
                    "cffobjs.c",
                    "cffgload.c",
                    "cffcmap.c",
                    "cidparse.c",
                    "cidload.c",
                    "cidobjs.c",
                    "cidriver.c",
                    "cidgload.c",
                    "gxvfeat.c",
                    "gxvcommn.c",
                    "gxvbsln.c",
                    "gxvtrak.c",
                    "gxvjust.c",
                    "gxvmort.c",
                    "gxvmort0.c",
                    "gxvmort1.c",
                    "gxvmort2.c",
                    "gxvmort4.c",
                    "gxvmort5.c",
                    "gxvmorx.c",
                    "gxvmorx0.c",
                    "gxvmorx1.c",
                    "gxvmorx2.c",
                    "gxvmorx4.c",
                    "gxvmorx5.c",
                    "gxvkern.c",
                    "gxvopbd.c",
                    "gxvprop.c",
                    "gxvlcar.c",
                    "gxvmod.c",
                    "zutil.c",
                    "inftrees.c",
                    "infutil.c",
                    "infcodes.c",
                    "infblock.c",
                    "inflate.c",
                    "adler32.c",
                    "ftzopen.c",
                    "otvbase.c",
                    "otvcommn.c",
                    "otvgdef.c",
                    "otvgpos.c",
                    "otvgsub.c",
                    "otvjstf.c",
                    "otvmath.c",
                    "otvmod.c",
                    "pcfutil.c",
                    "pcfread.c",
                    "pcfdrivr.c",
                    "pfrload.c",
                    "pfrgload.c",
                    "pfrcmap.c",
                    "pfrobjs.c",
                    "pfrdrivr.c",
                    "pfrsbit.c",
                    "psobjs.c",
                    "psauxmod.c",
                    "t1decode.c",
                    "t1cmap.c",
                    "afmparse.c",
                    "psconv.c",
                    "pshpic.c",
                    "pshrec.c",
                    "pshglob.c",
                    "pshalgo.c",
                    "pshmod.c",
                    "pspic.c",
                    "psmodule.c",
                    "rastpic.c",
                    "ftraster.c",
                    "ftrend1.c",
                    "sfntpic.c",
                    "ttload.c",
                    "ttmtx.c",
                    "ttcmap.c",
                    "ttkern.c",
                    "sfobjs.c",
                    "sfdriver.c",
                    "ttsbit.c",
                    "ttpost.c",
                    "ttbdf.c",
                    "ttsbit0.c",
                    "ftspic.c",
                    "ftgrays.c",
                    "ftsmooth.c",
                    "ttpic.c",
                    "ttdriver.c"   ,
                    "ttpload.c"    ,
                    "ttgload.c"    ,
                    "ttobjs.c"     ,
                    "ttinterp.c",
                    "ttsubpix.c",
                    "ttgxvar.c"    ,
                    "t1parse.c",
                    "t1load.c",
                    "t1objs.c",
                    "t1driver.c",
                    "t1gload.c",
                    "t1afm.c",
                    "t42objs.c",
                    "t42parse.c",
                    "t42drivr.c",

                    'gxvalid',
                    'src/tools',
                    #'src/pcf',
                ]
            )
            #and not ('src/base' in f and not 'ftbase.c' in f)
            #and not ('src/cache' in f and not 'ftcache.c' in f)
            #and not ('src/autofit' in f and not 'autofit.c' in f)
            #and not ('src/autofit' in f and not 'autofit.c' in f)
        ),
        directory = 'release/lib',
        defines = ['FT2_BUILD_LIBRARY'],
    )
    #freetype = c.libraries.FreetypeLibrary(compiler, shared=True)

    graphic_libraries = (
        sdl.libraries +
        opengl.libraries +
        #freetype.libraries
        [freetype]
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

    libetc = compiler.link_library(
        'libetc',
        rglob("src/etc/*.cpp"),
        directory  = 'release/lib',
        libraries = base_libraries + boost.libraries,
        defines = ['ETC_BUILD_DYNAMIC_LIBRARY'],
        shared = True #and not platform.IS_MACOSX #bug with tup on macosx
    )

    libglew = compiler.link_static_library(
        'libglew',
        ['src/glew/glew.c'],
        directory = 'release/lib',
        libraries = graphic_libraries,
        defines = ['GLEW_NO_GLU', 'GLEW_STATIC'],
    )

    graphic_libraries.insert(0, libglew)

################### libcube

    libcube = compiler.link_dynamic_library(
        'libcube',
        rglob("src/cube/*.cpp"),
        directory  = 'release/lib',
        libraries=[libetc] + graphic_libraries + boost.libraries + python.libraries,
    )


    boost_python_pch = compiler.generate_precompiled_header(
        "src/wrappers/boost/python.hpp",
        libraries=[libcube, libetc] + graphic_libraries + boost.libraries + python.libraries + base_libraries,
    )

    for binding in rglob("cube/*.py++", dir='src'):
        t = compiler.link_dynamic_library(
            path.splitext(path.basename(binding))[0],
            [binding],
            ext = python.ext,
            directory = path.dirname("release/lib/python", binding[4:]),
            libraries=[libcube, libetc] + graphic_libraries + boost.libraries + python.libraries + base_libraries,
            precompiled_headers = [boost_python_pch],
        )
        #t.additional_inputs.append(boost_python_pch)
        #for object_target in t.dependencies[0].dependencies:
        #    object_target.additional_inputs.append(boost_python_pch)

################### libcubeapp

    libcubeapp = compiler.link_static_library(
        'libcubeapp',
        (src for src in rglob("src/cubeapp/*.cpp") if not src.endswith('main.cpp')),
        directory  = 'release/lib',
        libraries = [libcube, libetc] + graphic_libraries + boost.libraries + python.libraries,
    )

    for binding in rglob("cubeapp/*.py++", dir='src'):
        compiler.link_dynamic_library(
            path.splitext(path.basename(binding))[0],
            [binding],
            ext = python.ext,
            directory = path.dirname("release/lib/python", binding[4:]),
            libraries=[libcubeapp, libcube, libetc] + graphic_libraries + boost.libraries + python.libraries + base_libraries,
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
        )


