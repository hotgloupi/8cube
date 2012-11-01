# -*- encoding: utf-8 -*-

from cube import gl
from cubeapp import core

from .bindings import BINDINGS
from .gui import MainMenu
from .player import Player

class Game(core.Game):

    def __init__(self, window, client):
        self._menu = MainMenu(renderer=window.renderer, game=self)
        super(Game, self).__init__(window, client, BINDINGS)
        self._player = Player(client, self.inputs)

    @property
    def gui(self):
        return self._menu

    def on_frame(self, painter):
        painter.state.view = gl.matrix.look_at(
            self.player.camera.eye,
            self.player.camera.look,
            self.player.camera.up
        )
