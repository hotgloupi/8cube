# -*- encoding: utf-8 -*-

from cube import system
from cube.gui.widgets.viewport import Viewport

class RootWindow(system.Window):

    def __init__(self, title, width, height):
        super(RootWindow, self).__init__(title, width, height)
        self._root_widget = Viewport(width, height)
        print(self.renderer.description())

    @property
    def root_widget(self):
        return self._root_widget

    @root_widget.setter
    def root_widget(self, w):
        assert isinstance(w, Widget)
        self._root_widget = widget

    def connect(self, event, callback):
        return getattr(self, 'connect_' + event)(callback)

    def render(self):
        if self._root_widget is not None:
            self.renderer.swap_buffers()
            self._root_widget.render(self.renderer)


