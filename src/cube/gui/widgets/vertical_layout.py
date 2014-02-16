# -*- encoding: utf-8 -*-

from .. import view

class VerticalLayout(view.View):

    def __init__(self, **kwargs):
        kwargs.setdefault('tag', 'vlayout')
        super(VerticalLayout, self).__init__(**kwargs)

    def on_resize(self, w, h):
        pass
