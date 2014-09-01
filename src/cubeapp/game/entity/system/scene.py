

from . import System

import cube

class Scene(System):

    def __init__(self, scene):
        super().__init__()
        self.scene = scene
        from .. import component
        self.__init_methods = {
            component.Transform: self.__init_transform,
            component.Drawable: self.__init_content,
            component.Bindable: self.__init_content,
        }


    def init_component(self, entity, component, **kw):
        self.__init_methods[component.__class__](entity, component, **kw)

    def __init_transform(self,
                         entity,
                         component,
                         parent_node = None,
                         matrix = None):
        if parent_node is None:
            parent_node = self.scene.graph.root
        if matrix is not None:
            component.matrix = matrix
        component.node = parent_node.emplace_child(
            cube.scene.Transform,
            'transform-' + entity.name,
            component.matrix
        )
        return component

    def __init_content(self, entity, component, parent_node = None):
        assert component.content is not None
        if parent_node is None:
            for c in reversed(entity.components):
                if Scene in c.systems:
                    parent_node = c.node
                    break
            if parent_node is None:
                parent_node = self.scene.graph.root

        component.node = parent_node.emplace_child(
            cube.scene.content_node_type(component.content),
            entity.name + '-' + component.content.__class__.__name__.lower(),
            component.content
        )

    def shutdown_component(self, entity, component):
        self.scene.graph.remove(component.node)
