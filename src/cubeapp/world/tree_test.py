
from .tree import Tree, find_nodes
from cube import gl, units

from cube.test import Case

class _(Case):

    def test_init(self):
        t = Tree(60)

        for s in [-1, ]:
            try:
                t = Tree(s)
            except:
                pass
            else:
                self.fail("Should have thrown")

    def test_find_nodes(self):
        pos = gl.vec3d()
        c = gl.Camera()
        c.look_at(gl.vec3f(1, 1, 0))
        c.init_frustum(
            units.deg(45), 640 / 480, 0.005, 1
        )
        tree = Tree(62)
        for n in find_nodes(tree, pos, c.frustum):
            print(n)
