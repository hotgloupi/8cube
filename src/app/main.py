# -*- encoding: utf-8 -*-

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

def main(args):
    print("app.main")
    if not args:
        app = application.Application()
        return app.run()
    else:
        eval(args[0] + "()")

