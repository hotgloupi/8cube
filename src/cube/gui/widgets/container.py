# -*- encoding: utf-8 -*-

from cube.gui.widget import Widget

class Container(Widget):

    def __init__(self, tag, id_=None, class_=None):
        super(Container, self).__init__(tag, id_=id_, class_=class_)
        self._childs = []

    @property
    def parent(self):
        return super(Container, self).parent

    @parent.setter
    def parent(self, parent):
        old_parent = super(Container, self).parent
        if old_parent is not None:
            old_parent._remove_child(self)
        super(Container, self).parent = parent
        if parent is not None:
            parent._add_child(self)

    def _remove_child(self, child):
        assert(child is not None)
        assert(child in self._childs)
        idx = self._childs.index(child)
        if idx is not None:
            self._childs.pop(idx)

    def _add_child(self, child):
        assert(child is not None)
        assert(child not in self._childs)
        self._childs.append(child)

    def add_child(self, child):
        child.parent = self

    def render(self, painter):
        for child in self._childs:
            child.render(painter)
