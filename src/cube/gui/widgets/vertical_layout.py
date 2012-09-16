# -*- encoding: utf-8 -*-

from .container import Container

class VerticalLayout(Container):

    def __init__(self, **kwargs):
        kwargs.setdefault('tag', 'vlayout')
        super(VerticalLayout, self).__init__(**kwargs)
