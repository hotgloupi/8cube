from cube.test import Case
from . import Graph, Node

class _(Case):

    def test_init(self):
        n = Node("LOL")
        self.assertIsInstance(n, Node)
        self.assertEqual(n.name, "LOL")
        self.assertEqual(str(n), "<Node LOL>")
        n.name = "MEGALOL"
        self.assertEqual(n.name, "MEGALOL")
        self.assertEqual(str(n), "<Node MEGALOL>")

    def test_visitor(self):
        visited = []
        visitor = lambda n: visited.append(n.name)
        n = Node("LOL")
        n.visit(visitor)
        self.assertIn(n.name, visited)
        self.assertEqual(len(visited), 1)

    def test_throwing_visitor(self):
        def visitor(n):
            raise Exception("COUCOU")

        try:
            Node("test").visit(visitor)
        except Exception as e:
            self.assertEqual(str(e), "COUCOU")
        else:
            self.fail("Should have thrown COUCOU")

    def test_emplace(self):
        g = Graph()
        self.assertEqual(g.size, 1)
        child = g.root.emplace_child(Node, "lol")
        self.assertEqual(child.name, "lol")
        self.assertEqual(g.size, 2)

