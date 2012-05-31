# -*- encoding: utf-8 -*-

import cube

class Widget:

    def __init__(self, tag, id_=None, class_=None):
        self._parent = None
        self._childs = []
        self._tag = tag
        self._id_ = id_
        self._class_ = class_
        self._background_color = cube.gl.Color4f("white");
        self._foreground_color = cube.gl.Color4f("black");
        self._position = cube.gl.Vector2f();
        self._size = cube.gl.Vector2f()
        attrs = [
            'tag',
            'id_',
            'class_',
            'background_color',
            'foreground_color',
            'position',
            'size',
        ]
        for attr in attrs:
            setattr(
                self, "attr",
                property(
                    lambda s: getattr(s, '_' + attr),
                    lambda s, v: setattr(s, '_' + attr, v)
                )
            )

    @property
    def parent(self): return self._parent

    @parent.setter
    def parent(self, parent):
        if self._parent is not None:
            self._parent._removeChild(self)
        self._parent = parent
        if self._parent is not None:
            self._parent._addChild(self)


    def _removeChild(self, child):
        assert(child is not None)
        assert(child in self._childs)
        idx = self._childs.index(child)
        if idx is not None:
            self._childs.pop(idx)

    def _addChild(self, child):
        assert(child is not None)
        assert(child not in self._childs)
        self._childs.append(child)

    def addChild(self, child):
        child.parent = self

    def _render_childs(self, renderer):
        for child in self._childs:
            child.render(renderer)

    def render(self, renderer):
        self._render_childs(renderer)
