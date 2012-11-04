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

    @property
    def player(self):
        return self._player

    def render(self):
        with self.renderer.begin(gl.mode_3d) as painter:
            painter.state.projection = self.__projection_matrix
            painter.state.view = gl.matrix.look_at(
                self.player.camera.position,
                self.player.camera.position + self.player.camera.front,
                self.player.camera.up
            )
            painter.bind(self.__sp)
            painter.bind(self.__vb)
            painter.draw_elements(gl.DrawMode.quads, self.__indices, 0, 4)
