# -*- encoding: utf-8 -*-


from . import container

class Viewport(container.Container):

    def __init__(self, x, y, w, h, id_=None, class_=None):
        super(Viewport, self).__init__("viewport", id_=id_, class_=class_)

    def render(self, painter):
        print("Render viewport", self._childs)
        super(Viewport, self).render(painter)
