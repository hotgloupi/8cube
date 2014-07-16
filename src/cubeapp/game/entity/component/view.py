
from .component import Component
from ..system import Renderer
import cube

class View(Component):
    name = 'view'
    systems = [Renderer]

    def init(self, contents = []):
        if self.nodes is not None:
            raise Exception("Node reuse not supported yet")
        self.nodes = []
        if self.parent_node is None:
            self.parent_node = self.manager.scene.graph.root
        prev_node = self.parent_node
        for content in contents:
            prev_node = prev_node.emplace_child(
                cube.scene.content_node_type(content),
                self.entity.name + '-' + content.__class__.__name__.lower(),
                content,
            )
            self.nodes.append((prev_node, content))

