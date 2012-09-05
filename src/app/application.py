# -*- encoding: utf-8 -*-

import cube

class Application(cube.gui.Application):

    def __init__(self):
        super(Application, self).__init__(
            name="Infinit-cube",
        )
        self.__prepare()

    def __prepare(self):
        b = cube.gui.widgets.Button()
        self.viewport.add_child(b)
