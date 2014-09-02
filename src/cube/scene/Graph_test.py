
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


    def test_remove_node(self):
        g = Graph()
        n1 = g.root.insert_child(Node("n1"))
        n2 = g.root.insert_child(Node("n2"))
        n3 = g.root.insert_child(Node("n3"))
        self.assertEqual(g.size, 4)
        g.remove(n1)
        g.remove(n2)
        g.remove(n3)
        self.assertEqual(g.size, 1)

    def test_remove_node_backward(self):
        g = Graph()
        n1 = g.root.insert_child(Node("n1"))
        n2 = g.root.insert_child(Node("n2"))
        n3 = g.root.insert_child(Node("n3"))
        self.assertEqual(g.size, 4)
        g.remove(n3)
        g.remove(n2)
        g.remove(n1)
        self.assertEqual(g.size, 1)

