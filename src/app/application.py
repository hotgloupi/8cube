# -*- encoding: utf-8 -*-

import os
import cube

from . import core

class Application(cube.gui.Application):

    def __init__(self, game_directories=[], game=None):
        super(Application, self).__init__(
            name="Infinit-cube",
        )
        game_dir = None
        for dir_ in game_directories:
            if os.path.isdir(os.path.join(dir_, game)):
                game_dir = dir_
                break
        if game_dir is None:
            raise Exception("Couldn't find a game named '%s'" % str(game))

        self._client = core.Client()
        self._game = core.load_game(game_dir, game,
                                    self.window,
                                    self._client)

        self.__prepare()
        self.window.inputs.on_keydown.connect(self._on_keydown)

    def __prepare(self):

        self._main_menu = cube.gui.widgets.VerticalLayout(
            renderer=self.window.renderer
        ).add_child(
            cube.gui.widgets.Button("New game", x=10, y=100, w=400, h=90)
        )
        self._game_menu = self._game.gui
        self.viewport.add_child(
            self._game_menu
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

    def _on_quit(self):
        self.stop()

