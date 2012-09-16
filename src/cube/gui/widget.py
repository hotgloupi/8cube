# -*- encoding: utf-8 -*-

import cube
from .stylesheet import Stylesheet

def _make_default_stylesheet():
    stylesheet = Stylesheet()
    stylesheet.set_styles([
        ("font-size", "48px"),
        ("font-file", "/usr/share/fonts/truetype/freefont/FreeMono.ttf"),
        ("font-color", "black"),
        ("background-color", "transparent"),
    ])
    return stylesheet

DEFAULT_STYLESHEET = _make_default_stylesheet()

class Widget:

    _registered_ids = set()
    _widget_count = {}

    def __init__(self, tag=None, id_=None, class_=None, renderer=None,
                 x=0, y=0, w=0, h=0):
        if not tag:
            raise Exception("Invalid tag name")
        self._tag = tag
        self._id = self._gen_id(tag, id_)
        self._class = class_
        self._background_color = cube.gl.Color4f("white");
        self._foreground_color = cube.gl.Color4f("black");
        self._position = cube.gl.Vector2f(x, y);
        self._size = cube.gl.Vector2f(w, h)
        self._stylesheet = DEFAULT_STYLESHEET

        self.__renderer = renderer
        self.__parent = None
        if self.__renderer is not None:
            self._prepare(self.__renderer)

    @classmethod
    def _gen_id(cls, tag, id_):
        cls._widget_count.setdefault(tag, 0)
        cls._widget_count[tag] += 1
        if id_:
            if id_ in cls._registered_ids:
                raise Exception("The widget id '" + id + "' is already used")
        else:
            id_ = tag + str(cls._widget_count[tag])
        cls._registered_ids.add(id_);
        return id_

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
                self.__parent = parent # we set it before the _prepare call
                self._prepare(parent.renderer)
                return
        self.__parent = parent

    @property
    def renderer(self):
        if self.__renderer is None:
            if self.__parent is None:
                raise Exception("Orphan widget built without renderer :(")
            self.__renderer = self.__parent.renderer
            assert self.__renderer is not None
        return self.__renderer

    @property
    def font(self):
        return self._stylesheet.get_font(
            self.renderer,
            self._tag,
            self._id,
            self._class
        )

    def render(self, painter):
        raise Exception("render method not implemented for this widget")

    def on_resize(self, w, h):
        pass

_attrs = [
    'tag',
    'id',
    'class',
    'background_color',
    'foreground_color',
    'position',
    'size',
    'stylesheet',
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
