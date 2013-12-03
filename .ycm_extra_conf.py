# -*- encoding: utf-8 -*-
import os, sys
import ycm_core

#SCRIPT_FILE = os.path.abspath(__file___)
#SCRIPT_DIR = os.path.dirname(SCRIPT_FILE)
SCRIPT_DIR = os.getcwd()

# These are the compilation flags that will be used in case there's no
# compilation database set (by default, one is not set).
# CHANGE THIS LIST OF FLAGS. YES, THIS IS THE DROID YOU HAVE BEEN LOOKING FOR.


include_dirs = [
    'build/src',
    'src',
    'src/glew',
    'deps/SDL/include',
    'deps/boost',
    'deps/freetype2/include',
    'build/dependencies/SDL/install/include/SDL2',
    'build/dependencies/SDL_image/install/include/SDL2',
    'deps/assimp/include',
    'deps/glm',
]

if sys.platform.startswith('win32'):
    include_dirs.extend([
        'c:/Python33/Include',
        "c:\\mingw\\x32-4.8.1-posix-dwarf-rev5\\mingw32\\lib\\gcc\\i686-w64-mingw32\\4.8.1\\include\\c++\\i686-w64-mingw32",
        "c:\\mingw\\x32-4.8.1-posix-dwarf-rev5\\mingw32\\lib\\gcc\\i686-w64-mingw32\\4.8.1\\include\\c++",
        "c:\\mingw\\x32-4.8.1-posix-dwarf-rev5\\mingw32\\lib\\gcc\\i686-w64-mingw32\\4.8.1\\include",
        "c:\\mingw\\x32-4.8.1-posix-dwarf-rev5\\mingw32\\i686-w64-mingw32\\include",
        "c:\\LLVM-3.4.svn\\lib\\clang\\3.4\\include",
    ])
elif sys.platform.startswith('darwin'):
    include_dirs.extend([
        '/Users/hotgloupi/local/lib/c++/v1',
    ])
elif sys.platform.startswith('linux'):
    include_dirs.extend([
        '/usr/include/c++/4.8',
        '/usr/include/c++/4.7',
    ])

flags = [
    '-Wall',
    '-Wextra',
    '-std=c++11',
    '-DGLM_FORCE_CXX11',
    '-x', 'c++',
]

for d in include_dirs:
    flags.append('-I')
    flags.append(d)


# Set this to the absolute path to the folder (NOT the file!) containing the
# compile_commands.json file to use that instead of 'flags'. See here for
# more details: http://clang.llvm.org/docs/JSONCompilationDatabase.html
#
# Most projects will NOT need to set this to anything; you can just change the
# 'flags' list of compilation flags. Notice that YCM itself uses that approach.
compilation_database_folder = ''

if compilation_database_folder:
  database = ycm_core.CompilationDatabase( compilation_database_folder )
else:
  database = None

def DirectoryOfThisScript():
  return os.path.dirname( os.path.abspath( __file__ ) )


def MakeRelativePathsInFlagsAbsolute( flags, working_directory ):
  if not working_directory:
    return list( flags )
  new_flags = []
  make_next_absolute = False
  path_flags = [ '-isystem', '-I', '-iquote', '--sysroot=' ]
  for flag in flags:
    new_flag = flag

    if make_next_absolute:
      make_next_absolute = False
      if not flag.startswith( '/' ):
        new_flag = os.path.join( working_directory, flag )

    for path_flag in path_flags:
      if flag == path_flag:
        make_next_absolute = True
        break

      if flag.startswith( path_flag ):
        path = flag[ len( path_flag ): ]
        new_flag = path_flag + os.path.join( working_directory, path )
        break

    if new_flag:
      new_flags.append( new_flag )
  return new_flags


def FlagsForFile( filename ):
  if database:
    # Bear in mind that compilation_info.compiler_flags_ does NOT return a
    # python list, but a "list-like" StringVec object
    compilation_info = database.GetCompilationInfoForFile( filename )
    final_flags = MakeRelativePathsInFlagsAbsolute(
      compilation_info.compiler_flags_,
      compilation_info.compiler_working_dir_ )

    # NOTE: This is just for YouCompleteMe; it's highly likely that your project
    # does NOT need to remove the stdlib flag. DO NOT USE THIS IN YOUR
    # ycm_extra_conf IF YOU'RE NOT 100% YOU NEED IT.
    try:
      final_flags.remove( '-stdlib=libc++' )
    except ValueError:
      pass
  else:
    relative_to = DirectoryOfThisScript()
    final_flags = MakeRelativePathsInFlagsAbsolute( flags, relative_to )

  return {
    'flags': final_flags,
    'do_cache': True
  }
