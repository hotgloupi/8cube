from cube.test import Case
from cube.scene import Graph, Node, visit

class _(Case):

    def test_init(self):
        n = Node("LOL")
        self.assertIsInstance(n, Node)
        self.assertEqual(n.name, "LOL")
        self.assertEqual(str(n), "<PythonNode LOL>")
        n.name = "MEGALOL"
        self.assertEqual(n.name, "MEGALOL")
        self.assertEqual(str(n), "<PythonNode MEGALOL>")

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

    def test_children(self):
        g = Graph()
        g.root.emplace_child(Node, "lol")
        self.assertEqual(g.root.children[0].name, "lol")

class RefCount(Case):

    def test_custom_node(self):
        class N(Node):
            pass
        n = N("lol")
        import gc
        self.assertEqual(len(gc.get_referrers(n)), 1)

class Inheritance(Case):

    def setUp(self):
        self.graph = Graph()

    def tearDown(self):
        self.graph = None
        import gc
        gc.collect()

    class MyNode(Node):
        def __init__(self):
            super().__init__("my node")
            self.lol = 42


    def test_emplace(self):
        my_node = self.graph.root.emplace_child(self.MyNode)
        self.assertEqual(my_node.name, "my node")
        self.assertEqual(my_node.lol, 42)

        my_node = self.graph.root.children[0]
        self.assertEqual(my_node.name, "my node")
        self.assertEqual(my_node.lol, 42)

    def test_visit(self):
        my_node = self.graph.root.emplace_child(self.MyNode)
        visited = []
        my_node.visit(lambda n: visited.append(n))
        self.assertEqual(len(visited), 1)
        self.assertEqual(visited[0].name, 'my node')
        self.assertEqual(visited[0].lol, 42)

   # def test_traverse(self):
   #     self.graph.root.emplace_child(self.MyNode)
   #     visited = []
   #     self.graph.traverse(lambda n: visited.append(n))

    def test_dfs_bfs(self):
        self.graph.root.emplace_child(self.MyNode)
        visited = []
        visit.bfs(self.graph, lambda n: visited.append(n))
        self.assertEqual(visited[1].name, "my node")
        self.assertEqual(visited[1].lol, 42)

    def test_children(self):
        self.graph.root.emplace_child(self.MyNode)
        self.assertEqual(self.graph.root.children[0].name, "my node")
        self.assertEqual(self.graph.root.children[0].lol, 42)

