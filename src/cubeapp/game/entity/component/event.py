
from ..system import Event

from . import Component

class Publisher(Component):
    systems = [Event]

    def __init__(self, channels, name = None):
        super().__init__(name)
        self.channels = channels

class Subscriber(Component):
    systems = [Event]

    def __init__(self, channels, controllers, name = None):
        super().__init__(name)
        self.channels = channels
        self.controllers = controllers
