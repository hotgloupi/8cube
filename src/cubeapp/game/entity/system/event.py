
from . import System

class Event(System):
    """Add given components to the event_manager.

    Compatible components must have a "channels" list attribute and should
    be callable with (event, elapsed_time) paremeters.
    """

    def __init__(self, event_manager, name = None):
        super().__init__(name)
        self.event_manager = event_manager

    def init_component(self, entity, component):
        self.event_manager.add(component)

    def shutdown_component(self, entity, component):
        self.event_manager.unregister(component)
