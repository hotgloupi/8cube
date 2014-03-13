# -*- encoding: utf-8 -*-

import os, sys

def console():
    import cube
    locals_ = {
        'cube': cube,
    }
    import code
    readline = None
    for m in ['readline', 'pyreadline']:
        try:
            readline = __import__(m)
        except:
            pass
    if readline is None:
        cube.warn("Couldn't import any readline module")
    else:
        import rlcompleter
        readline.set_completer(rlcompleter.Completer(locals_).complete)
        readline.parse_and_bind("tab: complete")

    code.InteractiveConsole(locals_).interact(
        banner="Type 'help(cube)' to get started."
    )

def parse_args(args):
    import argparse
    import textwrap
    class ArgFormatter(argparse.HelpFormatter):
        def _split_lines(self, text, width):
            return textwrap.dedent(text).split('\n')


    parser = argparse.ArgumentParser(
        description = "Launch a cube game",
        prog = "8cube",
        formatter_class = ArgFormatter
    )
    parser.add_argument(
        'game',
        help = "Specify a game's path or name",
        nargs = '?',
    )
    parser.add_argument(
        '--game-directories', '-G',
        action = "append",
        default = [],
        help = "Specify additional search directories for games",
    )
    parser.add_argument(
        '--console', '-c',
        action = "store_true",
        help = "Start a python console instead of launching a game",
    )
    parser.add_argument(
        '--script', '-s',
        action = "store",
        help = "Eval a script file"
    )
    parser.add_argument(
        '--unittests', '-U',
        help = "Launch all unit tests",
        action = 'store_true'
    )

    parser.add_argument(
        '--unittest', '-u',
        help = 'Launch unit tests that match a given pattern',
        action = 'store'
    )

    profile_stats_columns = (
        ('cumulative', "cumulative time"),
        ('file', "file name"),
        ('ncalls', "call count"),
        ('pcalls', "primitive call count"),
        ('line', "line number"),
        ('name', "function name"),
        ('nfl', "name/file/line"),
        ('time', "internal time"),
        ('tottime', "internal time"),
    )
    def csv(arg):
        fields = arg.split(',')
        for f in fields:
            if f not in (k for k, d in profile_stats_columns):
                import_cube().log.error("'%s' is not a valid stat field" % f)
                sys.exit(1)
        return fields

    parser.add_argument(
        '--profile', '-P',
        type = csv,
        help = "Enable Profiling and sort by comma separated list of fields in:\n" \
        + ''.join(('\t* %s: %s\n' % (n, d)) for n, d in profile_stats_columns),
        action = "store",
        const = ('time', 'cumulative', 'ncalls'),
        metavar = 'time,cumulative,ncalls,...',
        nargs = '?',
    )

    parser.add_argument(
        '--profile-output', '-PO',
        help = 'Specify the profile output file',
        nargs = '?',
        action = 'store',
        default = '8cube.profile'
    )
    return parser, parser.parse_args(args=args)


def import_test_file(root_dir, path):
    import cube
    old_python_path = sys.path
    sys.path = [root_dir] + sys.path
    try:
        parts = os.path.relpath(path, start = root_dir).split(os.path.sep)
        cube.log.debug("Found test", '.'.join(parts))
        __import__('.'.join(parts))
    finally:
        sys.path = old_python_path

def import_tests(root_dir, lib):
    lib_dir = os.path.join(root_dir, lib)
    for rootpath, dirs, files in os.walk(lib_dir):
        for f in files:
            if f.endswith('_test.py'):
                path = os.path.join(rootpath, f)
                import_test_file(root_dir, path[:-3])

def import_cube():
    try:
        import cube
        return cube
    except Exception:
        import traceback
        traceback.print_exc()
        raise

import traceback
import gc

def main(args):
    cube = import_cube()
    sys.argv = ['cubeapp.main'] + args
    cube.info("Launching 8cube with args", args)
    cube.constants.application.name("8cube")

    # XXX remove that
    with cube.trace_info("Initializing the font manager"):
        cube.gui.FontManager.populate()

    try:
        parser, args = parse_args(args)
        if args.profile:
            import cProfile, pstats
            pr = cProfile.Profile()
            pr.enable()
            _main(parser, args)
            pr.disable()
            stats = pstats.Stats(pr)
            stats.sort_stats(*tuple(args.profile))
            stats.print_stats()
            stats.dump_stats(args.profile_output)
        else:
            _main(parser, args)
    except KeyboardInterrupt:
        return
    except cube.Exception as e:
        bt = e.backtrace[2:]
        index = -1
        for i, frame in enumerate(bt):
            if 'boost::python::detail::caller' in frame or \
               'boost::python::detail::invoke' in frame:
                index = i
                break
        if index > 0:
            bt = bt[:index]
        bt.reverse()

        print("Python traceback: (most recent call last)", file=sys.stderr)
        traceback.print_tb(e.__traceback__, file=sys.stderr)
        print("c++ traceback: (most recent call last)", file=sys.stderr)
        for i, frame in enumerate(bt):
            print('  %i: %s' % (i + 1, frame), file=sys.stderr)
        print(e, file=sys.stderr)
    #_main(args)
    gc.collect()

def _main(parser, args):
    import cube

    if args.unittests or args.unittest:
        def run():
            lib_dir = os.path.abspath(
                os.path.join(
                    os.path.dirname(__file__),
                    '..'
                )
            )
            for lib in ['cube', 'cubeapp']:
                import_tests(lib_dir, lib)

            cube.log.set_mode(cube.log.Mode.synchroneous)
            if args.unittest:
                pattern = '*%s*' % args.unittest
            else:
                pattern = '*'
            if cube.test.registry.run(pattern):
                cube.info("All tests passed")
    elif args.console:
        def run():
            cube.log.set_mode(cube.log.Mode.synchroneous)
            console()
    elif args.script:
        def run():
            if not os.path.exists(args.script):
                cube.fatal("Cannot find the file '%s'" % args.script)
                return
            import runpy
            runpy.run_path(args.script, run_name = '__main__')
    elif args.game is not None:
        def run():
            from os.path import realpath, isdir, exists, join, dirname, basename
            path = realpath(args.game)
            if isdir(path) and (
                exists(join(path, 'game.py')) or
                exists(join(path, 'game.so')) or
                exists(join(path, 'game.pyd')) or
                exists(join(path, 'game', '__init__.py'))
            ):
                game_directories = [dirname(path)]
                game_name = basename(path)
            else:
                game_directories, game_name = args.game_directories, args.game
            from . import application
            app = application.Application(
                game_directories = game_directories,
                game_name = game_name
            )
            return app.run()
    else:
        parser.print_usage()

    run()

