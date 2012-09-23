# -*- encoding: utf-8 -*-

from . import application
import sys

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

def main(args):
    import cube
    try:
        app = application.Application(args)
        return app.run()
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

