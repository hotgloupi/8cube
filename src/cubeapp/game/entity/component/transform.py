
import cube

from .component import Component
from ..system import Scene

class Transform(Component):
    systems = [Scene]
    def __init__(self, matrix = cube.gl.mat4f(), name = None):
        super().__init__(name)
        self.matrix = matrix
