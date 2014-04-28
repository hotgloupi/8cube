
import cube

from .component import Component

class View(Component):
    name = 'view'

    __slots__ = ('parent_node', 'nodes')

    def init(self, contents = []):
        if self.nodes is not None:
            raise Exception("Node reuse not supported yet")
        self.nodes = []
        if self.parent_node is None:
            self.parent_node = self.manager.scene.graph.root
        prev_node = self.parent_node
        for content in contents:
            if isinstance(content, cube.gl.Bindable):
                node_type = cube.scene.ContentNodeBindable
                node_value = content
            elif isinstance(content, cube.gl.Drawable):
                node_type = cube.scene.ContentNodeDrawable
                node_value = content
            elif hasattr(content, 'drawable'):
                node_type = cube.scene.ContentNodeDrawable
                node_value = content.drawable(self.manager.renderer)
            elif hasattr(content, 'bindable'):
                node_type = cube.scene.ContentNodeBindable
                node_value = content.bindable(self.manager.renderer)
            else:
                raise Exception("Unknown content type")
            prev_node = prev_node.emplace_child(
                node_type,
                self.entity.name + '-' + content.__class__.__name__.lower(),
                node_value,
            )
            self.nodes.append((prev_node, content))

