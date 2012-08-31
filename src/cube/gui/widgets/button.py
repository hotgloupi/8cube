# -*- encoding: utf-8 -*-

from .. import widget

class Button(widget.Widget):
    def __init__(self):
        super(Button, self).__init__("button")

    def render(self, painter):
        print("Render button")
