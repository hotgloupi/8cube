
from .Node import Node
from .Graph import Graph
from .visit import *

from cube.test import Case

class _(Case):

    def test_bfs(self):
        g = Graph()
        g.root \
            .emplace_child(Node, "first gen") \
            .emplace_child(Node, "second gen")
        visited = []
        bfs(g, lambda n: visited.append(n))
        self.assertEqual(len(visited), 3)
