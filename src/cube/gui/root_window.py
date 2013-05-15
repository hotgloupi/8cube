# -*- encoding: utf-8 -*-

import cube
from cube.gl.renderer import Renderer
from cube import gl
from cube import system
from cube.gui.widgets.viewport import Viewport

class RootWindow:

    def __init__(self, title, width, height):
        self.__window = system.create_window(title, width, height, gl.renderer.Name.OpenGL)
        self._root_widget = Viewport(
            renderer=self.renderer,
            x=0, y=0, w=width, h=height,
        )
        self._hdlrs = {}
        for e in ['expose', 'resize', 'keydown']:
            self._hdlrs[e] = getattr(self.inputs, 'on_' + e).connect(
                getattr(self, '_on_' + e)
            )
        self._new_viewport_size = None

    @property
    def size(self):
        return self._root_widget.size

    @property
    def renderer(self):
        return self.__window.renderer

    def confine_mouse(self, wrap):
        self.__window.confine_mouse(wrap)

    def poll(self):
        return self.__window.poll()

    @property
    def inputs(self):
        return self.__window.inputs

    @property
    def root_widget(self):
        return self._root_widget

    @root_widget.setter
    def root_widget(self, w):
        assert isinstance(w, Widget)
        self._root_widget = widget
        self._root_widget.parent = self

    def render(self, painter):
        self.renderer.clear(
            gl.renderer.BufferBit.color |
            gl.renderer.BufferBit.depth
        )
        if self._new_viewport_size is not None:
            w, h = self._new_viewport_size
            cube.debug("Updating viewport size")
            self.renderer.viewport(0, 0, w, h);
            if self._root_widget:
                self._root_widget.on_resize(w, h)
            self._new_viewport_size = None
        if self._root_widget is not None:
            self._root_widget.render(painter)
        self.__window.swap_buffers()

    def swap_buffers(self):
        self.__window.swap_buffers()

    def _on_expose(self, w, h):
        cube.debug("Expose from window", w, h)
        self._new_viewport_size = (w, h)

    def _on_resize(self, w, h):
        cube.debug("resize from window", w, h)
        self._new_viewport_size = (w, h)

    def _on_keydown(self, mod, sym, k):
        cube.debug("Key down:", mod, sym, k, chr(k))

