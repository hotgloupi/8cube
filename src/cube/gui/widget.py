# -*- encoding: utf-8 -*-

import cube

class Widget:
    def __init__(self, tag, id_=None, class_=None, renderer=None,
                 x=0, y=0, w=0, h=0):
        self._tag = tag
        self._id_ = id_
        self._class_ = class_
        self._background_color = cube.gl.Color4f("white");
        self._foreground_color = cube.gl.Color4f("black");
        self._position = cube.gl.Vector2f(x, y);
        self._size = cube.gl.Vector2f(w, h)

        self.__renderer = renderer
        self.__parent = None
        if self.__renderer is not None:
            self._prepare(self.__renderer)

    def _prepare(self, renderer):
        """Prepare the widget for future rendering, should be overridden."""
        pass

    @property
    def parent(self):
        return self.__parent

    @parent.setter
    def parent(self, parent):
        if parent is not None:
            if self.__renderer is None and self.__parent is None:
                self._prepare(parent.renderer)
        self.__parent = parent

    @property
    def renderer(self):
        if self.__renderer is None:
            if self.__parent is None:
                raise Exception("Orphan widget built without renderer :(")
            self.__renderer = self.__parent.renderer
            assert self.__renderer is not None
        return self.__renderer

    def render(self, painter):
        raise Exception("render method not implemented for this widget")

    def on_resize(self, w, h):
        pass

_attrs = [
    'tag',
    'id_',
    'class_',
    'background_color',
    'foreground_color',
    'position',
    'size',
]
def _mkprop(attr):
    return property(
        lambda s: getattr(s, '_' + attr),
        lambda s, v: setattr(s, '_' + attr, v)
    )

for attr in _attrs:
    setattr(
        Widget, attr, _mkprop(attr)
    )
