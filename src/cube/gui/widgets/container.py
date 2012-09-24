# -*- encoding: utf-8 -*-

from .. import widget

class Container(widget.Widget):

    def __init__(self, **kwargs):
        self._childs = []
        super(Container, self).__init__(**kwargs)

    @property
    def parent(self):
        return super(Container, self).parent

    @parent.setter
    def parent(self, parent):
        old_parent = self.parent
        if old_parent is not None and isinstance(old_parent, Container):
            old_parent._remove_child(self)

        # XXX setter property does not work with the super class wrapper.
        # the following is equivalent to `super(Container, self).parent = parent`
        widget.Widget.parent.fset(self, parent)

        #if parent is not None:
        #    parent.add_child(self)

    def _remove_child(self, child):
        assert(child is not None)
        assert(child in self._childs)
        idx = self._childs.index(child)
        if idx is not None:
            self._childs.pop(idx)

    def add_child(self, child):
        return self.add_childs([child])

    def add_childs(self, childs):
        assert childs is not None
        for c in childs:
            assert(c not in self._childs)
            c.parent = self
        self._childs.extend(childs)
        return self

    def set_child(self, child):
        return self.set_childs([child])

    def set_childs(self, childs):
        for c in self._childs:
            c.parent = None
        self._childs = []
        return self.add_childs(childs)

    def render(self, painter):
        for child in self._childs:
            child.render(painter)

    def _get_children_size(self, size_hint):
        size = cube.gl.Vector2f()
        for child in self._childs:
            size += child.size_hints[size_hint]
        return size

    def on_resize(self, w, h):
        raise Exception("NotImplemented")
