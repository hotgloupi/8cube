# -*- encoding: utf-8 -*-

import cube

from . import gui
from . import core

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
        self._game_menu = gui.Game(
            renderer=self.window.renderer,
        )

        self._game_menu.game = core.Game(
            self.window,
            core.Client(),
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

    def _on_quit(self):
        self.stop()

