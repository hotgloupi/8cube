# -*- encoding: utf-8 -*-

from cube.gl import Color4f, vec2f
from cube.scene import Node
from .stylesheet import Stylesheet

class SizeHint:

    preferred = 0
    max_ = 1
    min_ = 2


class ComputedStyles:
    """Compute styles"""

    def __init__(self, view):
        for prop in ['color', 'background-color', 'border-color']:
            setattr(
                self,
                prop.replace('-', '_'),
                Color4f(view.style(prop))
            )

        for prop in ['margin', 'padding', 'border-size']:
            setattr(
                self,
                prop.replace('-', '_'),
                Stylesheet.to_pixel(view.style(prop))
            )


def _make_default_stylesheet():
    stylesheet = Stylesheet()
    stylesheet.set_styles([
        ("font-size", "16px"),
        ("font-family", 'Helvetica, Arial, Times, Courier, Droid'),
        ("font-style", "regular"),
        ("color", "black"),
        ("background-color", "transparent"),
    ])
    return stylesheet

DEFAULT_STYLESHEET = _make_default_stylesheet()

class View(Node):

    _registered_ids = set()
    _view_count = {}

    def __init__(self, tag=None, id_=None, class_=None, renderer=None,
                 x=0, y=0, w=0, h=0,
                 min_size=None, max_size=None, prefered_size=None):
        if not tag:
            raise Exception("Invalid tag name")
        self._tag = tag
        self._id = self._gen_id(tag, id_)
        self._class = class_
        self._position = vec2f(x, y)
        self._size = vec2f(w, h)
        self._stylesheet = DEFAULT_STYLESHEET
        self.__renderer = renderer
        if prefered_size is None:
            prefered_size = self._size
        self.__size_hints = self._compute_size_hints(
            prefered_size,
            max_size,
            min_size,
        )
        super().__init__(self._id)
        #self.reload_styles()

    @property
    def renderer(self):
        if self.__renderer is None:
            raise Exception("No renderer given when creating %s" % self)
        return self.__renderer

    def __del__(self):
        self._registered_ids.remove(self._id)
        del self._id
        del self._tag
        del self._stylesheet
        del self.__renderer

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
        cls._view_count.setdefault(tag, 0)
        cls._view_count[tag] += 1
        if id_:
            if id_ in cls._registered_ids:
                raise Exception("The view id '" + id + "' is already used")
        else:
            id_ = tag + str(cls._view_count[tag])
        assert id_ not in cls._registered_ids
        cls._registered_ids.add(id_)
        return id_

    @property
    def size_hints(self):
        return self.__size_hints

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

    @property
    def styles(self):
        return self.__computed_styles

    def reload_styles(self):
        self.__computed_styles = ComputedStyles(self)

    def render(self, painter):
        #raise Exception("render method not implemented for this view")
        pass

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
        View, attr, _mkprop(attr)
    )
