# -*- encoding: utf-8 -*-

from cube import gl
from cubeapp import core

from .bindings import BINDINGS
from .gui import MainMenu
from .player import Player

class Game(core.Game):

    def __init__(self, window, client):
        self._menu = MainMenu(renderer=window.renderer, game=self)
        world = core.World(core.world.Storage(),
                           core.world.Generator(),
                           window.renderer)
        super(Game, self).__init__(window, client, BINDINGS, world)
        self._player = Player(client, self.inputs)

    @property
    def gui(self):
        return self._menu

    @property
    def player(self):
        return self._player

    def render(self):
        with self.renderer.begin(gl.mode_3d) as painter:
            painter.state.projection = self.projection_matrix
            painter.state.view = gl.matrix.look_at(
                self.player.camera.position,
                self.player.camera.position + self.player.camera.front,
                self.player.camera.up
            )
            self.world.render(self.renderer)

