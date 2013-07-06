# -*- encoding: utf-8 -*-

from .color import Color3f
from .renderer import Bindable

class Material(Bindable):
    def __init__(self,
                 ambiant = Color3f("pink")):
        self.ambiant = ambiant


