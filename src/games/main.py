# -*- encoding: utf-8 -*-

import cube

class Application(cube.gui.Application):

    def __init__(self):
        super(Application, self).__init__()
        self.viewport.add_child(
            cube.gui.widgets.Button()
        )


def main(argv):
    app = Application()
    app.run()
