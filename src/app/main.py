# -*- encoding: utf-8 -*-

import sys

from . import application

def console():
    import cube
    locals_ = {
        'cube': cube,
    }
    import code
    import readline
    code.InteractiveConsole(locals_).interact(
        banner="Type 'help(cube)' to get started."
    )

def parse_args(args):
    import argparse
    parser = argparse.ArgumentParser(
        description="Launch a cube game",
        prog="8cube",
    )
    parser.add_argument(
        'game', help="Specify a game's name",
        nargs='?',
    )
    parser.add_argument(
        '--games-dir', '-G', action="append",
        help="Specify additional search directories for games",
    )
    parser.add_argument(
        '--console', '-c', action="store_true",
        help="Start a python console instead of launching a game",
    )
    return parser, parser.parse_args(args=args)

def main(args):
    import cube
    try:
        parser, args = parse_args(args)
        if (args.console):
            console()
            return

        if not args.game:
            print("No game specified on the command line", file=sys.stderr)
            parser.print_usage()
            sys.exit(1)

        app = application.Application(game_directories=args.games_dir, game=args.game)
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

        err = lambda *args: print(*args, file=sys.stderr)
        err("Python traceback: (most recent call last)")
        import traceback
        traceback.print_tb(e.__traceback__, file=sys.stderr)
        err("c++ traceback: (most recent call last)")
        for i, frame in enumerate(bt):
            err('  %i: %s' % (i + 1, frame))
        err(e)

