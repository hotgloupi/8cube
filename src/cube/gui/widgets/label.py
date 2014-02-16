# -*- encoding: utf-8 -*-

from .. import view
from cube import gl

class Label(view.View):

    def __init__(self, text, **kwargs):
        kwargs.setdefault('tag', 'label')
        super(Label, self).__init__(**kwargs)
        self.__str = text
        self.__text = gl.Text(self.font, self.__str)

    @property
    def text(self):
        return self.__str

    @text.setter
    def text(self, value):
        self.__str = value
        self.__text = gl.Text(self.font, self.__str);

    def render(self, painter):
        self.__text.draw(painter)

