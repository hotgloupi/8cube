# -*- encoding: utf-8 -*-

from .. import view

class Viewport(view.View):

    def __init__(self, **kwargs):
        kwargs.setdefault('tag', 'viewport')
        super(Viewport, self).__init__(**kwargs)

    def render(self, painter):
        super(Viewport, self).render(painter)

    def on_resize(self, w, h):
        """Forward event to its children."""
        #for child in self._childs:
        #    child.on_resize(w, h)
        pass
