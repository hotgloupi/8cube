
from cube.test import Case

from . import Graph, Node

class _(Case):

    def test_simple(self):
        g = Graph()
        self.assertIsInstance(g.root, Node)
        self.assertEqual(g.size, 1)

    def test_node_ctor(self):
        n = Node("LOL!")
        g = Graph(n)
        self.assertEqual(g.root.name, "LOL!")

    def test_custom_node_ctor(self):
        class C(Node):
            def __init__(self):
                super().__init__("Megalol")
                self.lol = 42
        c = C()
        g = Graph(c)
        self.assertEqual(g.root.name, "Megalol")
        self.assertEqual(g.root.lol, 42)

