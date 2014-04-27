
import cube

from .component import Component

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
                transformation
            )
        else:
            raise Exception("Node reuse not supported yet")


