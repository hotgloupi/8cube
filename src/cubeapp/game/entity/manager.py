import cube

from .entity import Entity
from .component import Component
from .system import System
from .system import Scene as SceneSystem
from .system import Renderer as RendererSystem
from .system import Event as EventSystem
from ..event import Channel

class Manager:

    def __init__(self, scene = None, renderer = None, event_manager = None, inputs = None):
        self.__entities = {}
        self.__components = {}
        self.__event_manager = event_manager
        self.__renderer = renderer
        self.__inputs = inputs
        self.__systems = {}
        self.__registered_systems = {}
        if scene is not None:
            self.add_system(SceneSystem(scene))
        if renderer is not None:
            self.add_system(RendererSystem(renderer))
        if event_manager:
            self.add_system(EventSystem(event_manager))
            event_manager.register(self.__on_tick, Channel('tick'))

    @property
    def scene(self): return self.__scene

    @property
    def event_manager(self): return self.__event_manager

    @property
    def renderer(self): return self.__renderer

    @property
    def inputs(self): return self.__inputs

    def add_system(self, system):
        assert isinstance(system, System)
        cls = system.__class__
        if cls in self.__registered_systems:
            if system != self.__registered_systems[cls]:
                raise Exception(
                    "Trying to register the same system '%s' twice with different instances" % cls
                )
            return
        self.__registered_systems[cls] = system
        system.entity_manager = self
        self.__systems[cls] = []

    def system(self, name):
        for system in self.__registered_systems.values():
            if system.name == name:
                return system

    def create(self, entity_or_cls, *args, **kw):
        """Add an entity.
        """
        if isinstance(entity_or_cls, Entity):
            entity = entity_or_cls
        elif isinstance(entity_or_cls, str):
            entity = type(entity_or_cls.capitalize(), (Entity,), {})()
        elif isinstance(entity_or_cls, type) and issubclass(entity_or_cls, Entity):
            entity = entity_or_cls(*args, **kw)
        else:
            raise Exception("entity must be a string, a subclass or an instance"
                            " of Entity, got '%s'" % entity_or_cls)
        entity.manager = self
        self.__entities[entity] = []
        return entity

    def __destroy(self, entity):
        """Called by an entity that is destroying itself."""
        for component in entity.components[:]:
            self.__unregister_component(entity, component)
        del self.__entities[entity]

    def __unregister_component(self, entity, component):
        """Called by an entity that remove a component."""
        assert entity in self.__entities
        assert component in self.__entities[entity]
        for system_cls in component.systems:
            system = self.__registered_systems[system_cls]
            system._System__shutdown_component(entity, component)
        self.__entities[entity].remove(component)

    def __register_component(self, entity, component, **kw):
        """Called by an entity that has a new component."""
        for system_cls in component.systems:
            assert system_cls in self.__registered_systems, "%s system is not registered" % system_cls
            system = self.__registered_systems[system_cls]
            component_kw = kw.pop(system.name, {})
            system._System__init_component(entity, component, **component_kw)
        assert not kw, "Some arguments were not used while initializing the " \
                       "component %s: %s" % (component, str(kw))
        self.__entities[entity].append(component)
        return component

    def components(self, entity):
        return self.__entities[entity]

    def __on_tick(self, ev, delta):
        for system in self.__registered_systems.values():
            system.update(delta)
