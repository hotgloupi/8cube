# -*- encoding: utf-8 -*-

import cube

class Application(cube.gui.Application):

    def __init__(self):
        super(Application, self).__init__(
            name="Infinit-cube",
        )
        self.__prepare()
        self.window.inputs.on_keydown.connect(self._on_keydown)

    def __prepare(self):
        self._main_menu = cube.gui.widgets.VerticalLayout(
            renderer=self.window.renderer
        ).add_child(
            cube.gui.widgets.Button("New game", x=10, y=100, w=400, h=90)
        )

        self._game_menu = cube.gui.widgets.VerticalLayout(
            renderer=self.window.renderer
        ).add_child(
            cube.gui.widgets.Button("Game menu", x=10, y=100, w=400, h=90)
        )
        self.viewport.add_child(
            self._main_menu
        )

    def _on_keydown(self, mod, sym, char):
        s = chr(char)
        if s == 'm':
            self.viewport.set_child(
                self._main_menu
            )
        elif s == 'g':
            self.viewport.set_child(
                self._game_menu
            )
        print(s)

