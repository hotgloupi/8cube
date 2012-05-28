# -*- encoding: utf-8 -*-

from cube import gl
from cube import system
from cube.gui.widgets.viewport import Viewport

class RootWindow(system.Window):

    def __init__(self, title, width, height):
        super(RootWindow, self).__init__(title, width, height)
        self._root_widget = Viewport(0, 0, width, height)
        self._hdlrs = {
            'expose': self.connect('expose', self._on_expose),
            'resize': self.connect('resize', self._on_resize),
        }

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
            with self.renderer.begin(gl.renderer.mode_2d) as painter:
                self._root_widget.render(painter)

    def _on_expose(self, w, h):
        print("Expose from window", w, h)
        self.renderer.viewport(0, 0, w, h);
        self.renderer.swap_buffers()

    def _on_resize(self, w, h):
        print("resize from window", w, h)


