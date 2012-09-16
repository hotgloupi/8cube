# -*- encoding: utf-8 -*-

from cube.gui.widgets import Viewport, Button, VerticalLayout

class Game(Viewport):

    game = None

    def __init__(self, **kwargs):
        kwargs.setdefault('tag', 'game')
        super(Game, self).__init__(**kwargs)

    def _prepare(self, renderer):
        layout = VerticalLayout()
        self.add_child(layout)
        layout.add_child(
            Button("Game menu", x=10, y=100, w=400, h=90)
        )

    def render(self, painter):
        if self.game is not None:
            self.game.render()
        super(Game, self).render(painter)

    def on_resize(self, w, h):
        if self.game:
            self.game.on_resize(w, h)
