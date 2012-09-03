# -*- encoding: utf-8 -*-


from . import container

class Viewport(container.Container):

    def __init__(self, x, y, w, h, **kwargs):
        super(Viewport, self).__init__("viewport", **kwargs)

    def render(self, painter):
        print("Render viewport", self._childs)
        super(Viewport, self).render(painter)
