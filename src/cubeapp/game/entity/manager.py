
from .entity import Entity
from .component import Component

class Manager:

    def __init__(self, scene = None, renderer = None, event_manager = None, inputs = None):
        self.__entities = {}
        self.__components = {}
        self.__scene = scene
        self.__event_manager = event_manager
        self.__registered_component_types = {}
        self.__renderer = renderer
        self.__inputs = inputs

    @property
    def scene(self): return self.__scene

    @property
    def event_manager(self): return self.__event_manager

    @property
    def renderer(self): return self.__renderer

    @property
    def inputs(self): return self.__inputs

    def entity_components(self, entity):
        return self.__entities[entity]

    def entity_component(self, entity, component_name):
        return self.entity_components(entity)[component_name]

    def add(self, entity_or_cls, *additional_components, **components_args):
        """Add an entity.
        """
        if isinstance(entity_or_cls, Entity):
            entity = entity_or_cls
        elif isinstance(entity_or_cls, str):
            entity = type(entity_or_cls.capitalize(), (Entity,), {})()
        elif isinstance(entity_or_cls, type) and issubclass(entity_or_cls, Entity):
            entity = entity_or_cls()
        else:
            raise Exception("entity must be a string, a subclass or an instance"
                            " of Entity, got '%s'" % entity_or_cls)
        entity.manager = self
        entity.components = list(entity.components) + list(additional_components)
        self.__add_instance(entity, components_args)
        return entity

    def __add_instance(self, entity, components_args):
        assert entity not in self.__entities
        entity_components = self.__entities[entity] = {}
        for cls in entity.components:
            if isinstance(cls, tuple):
                cls, mapping = cls
            else:
                mapping = {}
            component = self.__register_component(
                entity,
                cls,
                mapping,
                components_args
            )
            entity_components[component.name] = component

    def __register_component(self, entity, cls, mapping, init_args):
        if not issubclass(cls, Component):
            raise Exception("The component '%s' is not a Component subclass" % cls)
        if cls.name is None:
            raise Exception("The component '%s' should have a name" % cls)
        if not isinstance(cls.name, str):
            raise Exception("The component '%s' name's '%s' is not of type string" % (
                cls, cls.name
            ))

        instance = cls(manager = self, entity = entity)
        self.__init_component(instance, mapping, init_args)

        registered_cls = self.__registered_component_types.setdefault(
            cls.name,
            cls
        )
        if registered_cls is not cls:
            raise Exception(
                "The component '%s' name '%s' is already used by the component '%s'" % (
                    cls, cls.name, registered_cls
                )
            )
        self.__components.setdefault(cls.name, set()).add(instance)
        return instance

    def __init_component(self, component, mapping, components_args):
        for attr, src in mapping.items():
            if '.' in src:
                src_component_name, src_attr = src.split('.')
            else:
                src_component_name, src_attr = src, None
            src_component = self.__entities[component.entity].get(src_component_name)
            if src_component is None:
                raise Exception(
                    ("Cannot initialize component '%s' because it depends "
                     "on component '%s' for " "attribute '%s'") % (
                        component, src_component_name, attr
                    )
                )
            if src_attr is not None:
                if src_attr not in src_component.attributes:
                    raise Exception(
                        ("Cannot initialize component '%s' because it depends "
                         "on component attribute '%s.%s' for " "attribute '%s'") % (
                            component, src_component, src_attr, attr
                        )
                    )
                setattr(component, attr, getattr(src_component, src_attr))
            else:
                setattr(component, attr, src_component)

        component.init(**components_args.get(component.name, {}))
