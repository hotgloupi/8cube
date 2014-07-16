from ..system import Renderer
from ..system import Scene

from . import Component

class Bindable(Component):
    systems = [Renderer, Scene]
    def __init__(self, content = None, name = None):
        super().__init__(name)
        self.content = content
