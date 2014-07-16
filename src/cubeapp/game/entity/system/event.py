
from . import System

class Event(System):

    def __init__(self, event_manager, name = None):
        super().__init__(name)
        self.event_manager = event_manager

    def init_component(self, entity, component):
        self.event_manager.add(component.controller)

