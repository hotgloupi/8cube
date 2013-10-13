# -*- encoding: utf-8 -*-

from cube import gl
from cubeapp.game import Game as GameBase
from cubeapp.world import World, Storage, Generator

from .bindings import BINDINGS
from .gui import MainMenu
from .player import Player

class Game(GameBase):

    def __init__(self, window, client):
        self._menu = MainMenu(renderer = window.renderer, game = self)
        world = World(
            storage = Storage(),
            generator = Generator(),
            renderer = window.renderer
        )
        super().__init__(window, client, BINDINGS, world)
        self._player = Player(client, self.input_translator)

    @property
    def gui(self):
        return self._menu

    @property
    def player(self):
        return self._player

    def render(self):
        with self.renderer.begin(gl.mode_3d) as painter:
            painter.state.projection = self.projection_matrix
            painter.state.model = gl.matrix.look_at(
                self.player.camera.position,
                self.player.camera.position + self.player.camera.front,
                gl.vec3f(0, 1, 0)
            )
            self.world.render(painter)

