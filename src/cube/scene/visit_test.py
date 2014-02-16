
from .Node import Node
from .Graph import Graph
from .visit import *

from cube.test import Case

class _(Case):

    def test_bfs(self):
        g = Graph()
        first = g.root.emplace_child(Node, "first gen")
        second = g.root.emplace_child(Node, "second gen")

        third = first.emplace_child(Node, "third")
        fourth = second.emplace_child(Node, "fourth")

        third.emplace_child(Node, "fifth")
        fourth.emplace_child(Node, "sixth")

        visited = []
        bfs(g, lambda n: visited.append(n))
        self.assertEqual(len(visited), 7)

        # XXX TODO
        #self.assertEqual(g.root.name, "root")
        #self.assertEqual(g.root.children[0].name, "first gen")
        #self.assertEqual(g.root.children[0].children[0].name, "second gen")

        self.assertEqual(visited[0].name, "root")
        self.assertEqual(visited[1].name, "first gen")
        self.assertEqual(visited[2].name, "second gen")
        self.assertEqual(visited[3].name, "third")
        self.assertEqual(visited[4].name, "fourth")
        self.assertEqual(visited[5].name, "fifth")
        self.assertEqual(visited[6].name, "sixth")

    def test_dfs(self):
        g = Graph()
        n3 = g.root\
                .emplace_child(Node, "1")\
                .emplace_child(Node, "2")\
                .emplace_child(Node, "3")\

        n3.emplace_child(Node, "4")
        n3.emplace_child(Node, "5")
        g.root.emplace_child(Node, "6").emplace_child(Node, "7")
        visited = []
        dfs(g, lambda n: visited.append(n))
        self.assertEqual(len(visited), 8)

        self.assertEqual(visited[0].name, "root")
        for i in range(1, 8):
            self.assertEqual(visited[i].name, "%s" % i)
