# -*- encoding: utf-8 -*-

import cube

class Application(cube.gui.Application):

    def __init__(self):
        super(Application, self).__init__(
            name="Infinit-cube",
        )
        self.__prepare()

    def __prepare(self):
        self.viewport.add_childs(list(
            cube.gui.widgets.Button("Salut" + str(y), w=300, h=90, x=y/2, y=y)
            for y in range(0, 600, 100)
        ))
