# -*- encoding: utf-8 -*-


from cube.gui.widgets.container import Container

class Viewport(Container):

    def __init__(self, width, height, id_=None, class_=None):
        super(Container, self).__init__("viewport", id_=id_, class_=class_)
