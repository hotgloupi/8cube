
class System:
    """Implements fonctionality needed by components."""

    # Set when attached to an entity manager
    entity_manager = None

    def __init__(self, name = None):
        self.name = (name or self.__class__.__name__).lower()
        self.__components = set()

    def __init_component(self, entity, component, **kw):
        """Called by the Manager when a component is added to an entity."""
        self.init_component(entity, component, **kw)
        self.__components.add(component)

    def __shutdown_component(self, entity, component):
        """Called by the Manager when a component is removed from an entity."""
        self.shutdown_component(entity, component)
        self.__components.remove(component)

    def init_component(self, entity, component, **kw):
        """Overridable method called for each component added to an entity."""
        pass

    def shutdown_component(self, entity, component):
        """Overridable method called for each component removed from an entity."""
        pass

    def update(self, delta):
        """Overridable method called each tick."""
        pass


from .scene import Scene
from .renderer import Renderer
from .event import Event
