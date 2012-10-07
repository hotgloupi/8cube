# -*- encoding: utf-8 -*-

from cube import gl
from cubeapp import core
from cube.gui.widgets import Viewport, Button, VerticalLayout

from .bindings import BINDINGS

class GUI(Viewport):

    def __init__(self, **kwargs):
        kwargs.setdefault('tag', 'game')
        self.game = kwargs.pop('game')
        super(GUI, self).__init__(**kwargs)

    def _prepare(self, renderer):
        layout = VerticalLayout()
        self.add_child(layout)
        layout.add_child(
            Button("Game menu", x=10, y=100, w=400, h=90)
        )

    def render(self, painter):
        if self.game is not None:
            self.game.render()
        super(GUI, self).render(painter)

    def on_resize(self, w, h):
        if self.game:
            self.game.on_resize(w, h)

class Game(core.Game):

    def __init__(self, window, client):
        self._menu = GUI(renderer=window.renderer, game=self)
        super(Game, self).__init__(window, client, BINDINGS)

    @property
    def gui(self):
        return self._menu

    def on_frame(self, painter):
        print("BIET")
        painter.state.view = gl.matrix.look_at(
            self.player.camera.eye,
            self.player.camera.look,
            self.player.camera.up
        )
