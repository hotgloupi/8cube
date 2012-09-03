# -*- encoding: utf-8 -*-

import cube

class Widget:

    def __init__(self, tag, id_=None, class_=None, renderer=None):
        self.__parent = None
        self.__tag = tag
        self.__id_ = id_
        self.__class_ = class_
        self.__renderer = renderer
        self.__background_color = cube.gl.Color4f("white");
        self.__foreground_color = cube.gl.Color4f("black");
        self.__position = cube.gl.Vector2f();
        self.__size = cube.gl.Vector2f()
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
                    lambda s: getattr(s, '__' + attr),
                    lambda s, v: setattr(s, '__' + attr, v)
                )
            )
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


