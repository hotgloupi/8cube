# -*- encoding: utf-8 -*-

import cube
import time

class App(cube.gui.Application):

    def __init__(self):
        super(App, self).__init__(name = "Test app")
        self.window.root_widget.add_child(
            cube.gui.widgets.Button("pif", x=50, y=100, w=200, h=100)
        )


if __name__ == '__main__':
    app = App()
    app.run()
