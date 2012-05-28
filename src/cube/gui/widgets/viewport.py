# -*- encoding: utf-8 -*-


from cube.gui.widgets.container import Container

class Viewport(Container):

    def __init__(self, x, y, w, h, id_=None, class_=None):
        super(Viewport, self).__init__("viewport", id_=id_, class_=class_)
