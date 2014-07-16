from ..system import Event
from . import Component

class Controller(Component):
    systems = [Event]

    def __init__(self, controller, name = None):
        super().__init__(name)
        self.controller = controller
