# -*- encoding: utf-8 -*-

import cube
from .stylesheet import Stylesheet

class SizeHint:

    preferred = 0
    max_ = 1
    min_ = 2


class ComputedStyles:
    """Compute styles"""

    def __init__(self, widget):
        self.__widget = widget

    def reload(self):
        for prop in ['color', 'background-color', 'border-color']:
            setattr(
                self,
                prop.replace('-', '_'),
                cube.gl.Color4f(self.__widget.style(prop))
            )

        for prop in ['margin', 'padding', 'border-size']:
            setattr(
                self,
                prop.replace('-', '_'),
                Stylesheet.to_pixel(self.__widget.style(prop))
            )


def _make_default_stylesheet():
    stylesheet = Stylesheet()
    stylesheet.set_styles([
        ("font-size", "48px"),
        ("font-family", 'Helvetica, Arial, Times, Courier, Droid'),
        ("font-style", "regular"),
        ("color", "black"),
        ("background-color", "transparent"),
    ])
    return stylesheet

DEFAULT_STYLESHEET = _make_default_stylesheet()

class Widget:

    _registered_ids = set()
    _widget_count = {}

    def __init__(self, tag=None, id_=None, class_=None, renderer=None,
                 x=0, y=0, w=0, h=0,
                 min_size=None, max_size=None, prefered_size=None):
        if not tag:
            raise Exception("Invalid tag name")
        self._tag = tag
        self._id = self._gen_id(tag, id_)
        self._class = class_
        self._position = cube.gl.Vector2f(x, y);
        self._size = cube.gl.Vector2f(w, h)
        self._stylesheet = DEFAULT_STYLESHEET
        self.reload_styles()
        self.__renderer = renderer
        self.__parent = None
        if prefered_size is None:
            prefered_size = self._size
        self.__size_hints = self._compute_size_hints(
            prefered_size,
            max_size,
            min_size,
        )
        if self.__renderer is not None:
            self._prepare(self.__renderer)

    def _compute_size_hints(self, preferred, max_, min_):
        assert preferred is not None
        if max_ is None:
            max_ = preferred
        if min_ is None:
            min_ = preferred
        return {
            SizeHint.preferred: preferred,
            SizeHint.min_: min_,
            SizeHint.max_: max_,
        }

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
    def size_hints(self):
        return self.__size_hints

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

    def style(self, name):
        return self._stylesheet.get_style(
            name, self._id, self._class, self._tag
        )

    def styles(self):
        return self.__computed_styles

    def reload_styles(self):
        self.__computed_styles = ComputedStyles(self)

    def render(self, painter):
        raise Exception("render method not implemented for this widget")

    def on_resize(self, w, h):
        pass


_attrs = [
    'tag',
    'id',
    'class',
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
