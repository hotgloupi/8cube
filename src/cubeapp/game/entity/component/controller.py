from ..system import Event
from . import Component

class Controller(Component):
    systems = [Event]

    def __init__(self, controller, channels = None, name = None):
        super().__init__(name)
        self.controller = controller
        if channels is not None:
            self.channels = channels
        elif hasattr(controller, 'channels'):
            self.channels = controller.channels
        else:
            raise Exception("No channels were provided")

    def __call__(self, ev, delta):
        return self.controller(ev, delta)
