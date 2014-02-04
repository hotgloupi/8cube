
from cube.test import Case

from . import Graph, Node

class _(Case):

    def test_simple(self):
        g = Graph()
        self.assertIsInstance(g.root, Node)
        self.assertEqual(g.size, 1)

