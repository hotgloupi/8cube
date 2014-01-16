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
    parser = argparse.ArgumentParser(
        description = "Launch a cube game",
        prog = "8cube",
    )
    parser.add_argument(
        'game',
        help = "Specify a game's name",
        nargs = '?',
    )
    parser.add_argument(
        '--games-dir', '-G',
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
        help = "Launch unit tests",
        action = 'store_true'
    )

    parser.add_argument(
        '--unittest', '-u',
        help = 'Launch unit tests that match the argument',
        action = 'store'
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

def main(args):
    sys.argv = ['cubeapp.main'] + args
    try:
        import cube
    except Exception:
        import traceback
        traceback.print_exc()
        return
    cube.info("Launching 8cube with args", args)
    cube.constants.application.name("8cube")

    with cube.trace_info("Initializing the font manager"):
        cube.gui.FontManager.populate()
    try:
        parser, args = parse_args(args)
        if args.unittests or args.unittest:
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

            return
        if args.console:
            cube.log.set_mode(cube.log.Mode.synchroneous)
            console()
            return

        if args.script:
            if not os.path.exists(args.script):
                cube.fatal("Cannot find the file '%s'" % args.script)
                return
            import runpy
            runpy.run_path(args.script, run_name = '__main__')
            return

        if not args.game:
            args.game = 'test'
            #print("No game specified on the command line", file = sys.stderr)
            #parser.print_usage()
            #sys.exit(1)

        from . import application
        app = application.Application(
            game_directories = args.games_dir,
            game_name = args.game
        )
        return app.run()
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
        import traceback
        traceback.print_tb(e.__traceback__, file=sys.stderr)
        print("c++ traceback: (most recent call last)", file=sys.stderr)
        for i, frame in enumerate(bt):
            print('  %i: %s' % (i + 1, frame), file=sys.stderr)
        print(e, file=sys.stderr)

