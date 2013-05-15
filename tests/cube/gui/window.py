# -*- encoding: utf-8 -*-

import cube
import time

class App(cube.gui.Application):

    def __init__(self):
        super(App, self).__init__(name = "Test app")


if __name__ == '__main__':
    app = App()
    app.run()
