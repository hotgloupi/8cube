
from .component import Component

class Entity:
    """An entity is a simple container of components.
    """
    # Set when the entity is created
    manager = None

    def __init__(self, name = None, **kw):
        if name is None:
            name = self.__class__.__name__
        self.name = name.lower()
        self.__dict__.update(kw)

    def add_component(self, component, **kw):
        """Add a component to an entity.

        Keyword arguments are made of keys that must be names of existing
        systems, and values must be dictionaries, that are forwarded to the
        init method of systems that handles the specified component.
        """
        assert isinstance(component, Component)
        assert self.manager is not None, "Entity not registered"
        return self.manager._Manager__register_component(self, component, **kw)

    def destroy(self):
        self.manager._Manager__destroy(self)

    def remove_component(self, component):
        self.manager._Manager__unregister_component(self, component)

    @property
    def components(self):
        return self.manager.components(self)

    def component(self, name):
        res = []
        for c in self.components:
            if c.name == name:
                res.append(c)
        if not res:
            raise Exception("No component '%s' found in %s" % (name, self))
        if len(res) > 1:
            raise Exception("More than one component '%s' found in %s" % (name, self))
        return res[0]

from cube.test import Case

class _(Case):

    def test_init(self):
        e = Entity()
        self.assertEqual(e.name, 'entity')
        e = Entity(name = "Test")
        self.assertEqual(e.name, "test")

        class LOLEntity(Entity):
            pass
        self.assertEqual(LOLEntity().name, "lolentity")

