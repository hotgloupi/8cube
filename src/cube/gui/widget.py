# -*- encoding: utf-8 -*-

import cube

class Widget:

    def __init__(self, tag, id_=None, class_=None):
        self._parent = None
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
                self, attr,
                property(
                    lambda s: getattr(s, '_' + attr),
                    lambda s, v: setattr(s, '_' + attr, v)
                )
            )

    @property
    def parent(self):
        return self._parent

    @parent.setter
    def parent(self, parent):
        self._parent = parent

    def render(self, painter):
        pass
