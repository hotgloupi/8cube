# -*- encoding: utf-8 -*-

from cube.gui.widgets import Viewport, Button, VerticalLayout

class MainMenu(Viewport):

    def __init__(self, **kwargs):
        kwargs.setdefault('tag', 'game')
        self.game = kwargs.pop('game')
        super(MainMenu, self).__init__(**kwargs)

    def _prepare(self, renderer):
        layout = VerticalLayout()
        self.add_child(layout)
        layout.add_child(
            Button("Game menu", x=10, y=100, w=400, h=90)
        )

    def render(self, painter):
        if self.game is not None:
            self.game.render()
        super(MainMenu, self).render(painter)

    def on_resize(self, w, h):
        if self.game:
            self.game.on_resize(w, h)

    def on_keydown(self):
        pass
