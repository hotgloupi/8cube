
class Entity:
    """Root class for an entity.

    This class acts as a simple container of its components.
    You can set class level components  by inheriting an entity or set them
    at instance level.

    If an entity has both class level and instance level components, they will
    be added.
    """

    components = []

    # Set when added to a manager
    manager = None

    def __init__(self, *components, name = None):
        if name is None:
            name = self.__class__.__name__.lower()
        self.name = name
        self.components = list(self.components) + list(components)

    def component(self, name):
        return self.manager.entity_component(self, name)
