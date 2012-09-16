# -*- encoding: utf-8 -*-

from ..gl.font import Font

class StyleDefinition(dict):
    def __init__(self, id_, class_, tag):
        self.id_ = id_
        self.class_ = class_
        self.tag = tag
        super(StyleDefinition).__init__()

class Stylesheet:
    _fonts = None
    _definitions = None

    def __init__(self):
        self._fonts = {}
        self._definitions = []

    def get_font(self, renderer, id_, class_, tag):
        font_definition = (
            self.get_style("font-file", id_, class_, tag),
            self.get_style("font-color", id_, class_, tag),
            self.to_pixel(self.get_style("font-size", id_, class_, tag))
        )
        font = self._fonts.get(font_definition)
        if not font:
            font = self._fonts[font_definition] = Font(
                renderer,
                font_definition[0],
                font_definition[2]
            )
        return font

    def get_style(self, style, id_, class_, tag):
        best_match = -1
        ret = None

        for d in self._definitions:
            match = 0
            if d.id_ and d.id_ == id_:
                match += 1
            if d.class_ and d.class_ == class_:
                match += 1
            if d.tag and d.tag == tag:
                match += 1

            if match < best_match:
                continue
            if style in d:
                ret = d[style]
                best_match = match

        return ret

    def set_style(self, style, value, id_=None, class_=None, tag=None):
        d = StyleDefinition(id_, class_, tag)
        d[style] = value
        self._definitions.append(d)

    def set_styles(self, styles, id_=None, class_=None, tag=None):
        d = StyleDefinition(id_, class_, tag)
        for k, v in styles:
            d[k] = v
        self._definitions.append(d)

    @staticmethod
    def to_pixel(value):
        if 'px' in value:
            return int(value[:-2])
        raise Exception("Unknown unit of %f" % value)

