
import itertools

import cube

class Component:
    """Base class for every component.

    Components are designed to be aggregated in entities, they should form a
    flat inheritance tree. That is, inherit from another component only if you
    need have a clear gain in terms of code factorization.

    @warning: Do not use the standard constructor __init__() to intialize your
    component, use init() instead.

    Components are initialized when an entity is created:
        1) The entity is instanciated, it has a name and some components.
        2) For each listed component:
            * The component is instanciated (calling __init__()).
            * All slot are defined to either None.
            * The two slots "manager" and "entity" are set.
            * The attribute mapping, if any is applied.
            * The init() method is called with arguments given when creating
              the entity.
            * Register component type (if needed).
        3) The entity is registered by its manager.
    """

    # Unique identifier of the component
    name = None

    __slots__ = ('_attributes', 'manager', 'entity')

    def __init__(self, **kw):
        self._attributes = None
        for slot in self.attributes:
            setattr(self, slot, None)
        for k, v in kw.items():
            setattr(self, k, v)

    @property
    def attributes(self):
        if self._attributes is None:
            self._attributes = tuple(
                slot for slot in itertools.chain.from_iterable(
                    getattr(cls, '__slots__', []) for cls in type(self).__mro__
                ) if slot != '_attributes'
            )
        return self._attributes

    def init(self):
        pass

class View(Component):
    name = 'view'

    __slots__ = ('parent_node', 'nodes')

    def init(self, contents = []):
        if self.nodes is not None:
            raise Exception("Node reuse not supported yet")
        self.nodes = {}
        if self.parent_node is None:
            self.parent_node = self.manager.scene.graph.root
        prev_node = self.parent_node
        for content in contents:
            if isinstance(content, cube.gl.Material):
                node_type = cube.scene.ContentNodeBindable
                node_value = content.bindable(self.manager.renderer)
            elif isinstance(content, cube.gl.Mesh):
                node_type = cube.scene.ContentNodeDrawable
                node_value = content.view(self.manager.renderer)
            prev_node = prev_node.emplace_child(
                node_type,
                self.entity.name + '-' + node_type.__name__.lower(),
                node_value,
            )
            self.nodes[prev_node] = content

class Transform(Component):
    name = 'transform'
    __slots__ = ('parent_node', 'node')
    def init(self, transformation = cube.gl.mat4f()):
        if self.node is None:
            if self.parent_node is None:
                self.parent_node = self.manager.scene.graph.root
            self.node = self.parent_node.emplace_child(
                cube.scene.Transform,
                self.entity.name,
                cube.gl.mat4f()
            )
        else:
            raise Exception("Node reuse not supported yet")


