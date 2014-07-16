
class System:
    """Implements fonctionality needed by components."""

    # Set when attached to an entity manager
    entity_manager = None

    def __init__(self, name = None):
        self.name = (name or self.__class__.__name__).lower()
        self.__components = []

    def __init_component(self, entity, component, **kw):
        self.init_component(entity, component, **kw)
        self.__components.append(component)

    def init_component(self, entity, component, **kw):
        """Overridable method called for each component added to an entity."""
        pass

    def update(self, delta):
        """Overridable method called each tick."""
        pass

from .scene import Scene
from .renderer import Renderer
from .event import Event
