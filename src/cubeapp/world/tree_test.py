import pathlib
from .tree import Tree, Node, find_nodes
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

    def test_init_node(self):
        n = Node(gl.vec3il(1,2,3), 10)
        self.assertEqual(n.origin, gl.vec3il(1,2,3))
        self.assertEqual(n.size, 10)

    def test_hash_node(self):
        a = set({Node(gl.vec3il(1,2,3), 10)})
        self.assertIn(Node(gl.vec3il(1,2,3), 10), a)
        self.assertNotIn(Node(gl.vec3il(0,2,3), 10), a)
        self.assertNotIn(Node(gl.vec3il(1,2,3), 11), a)

    #def test_find_nodes(self):
    #    c = gl.Camera()
    #    pos = gl.vec3d(0, 0, 0)
    #    c.look_at(gl.vec3f(10.0, 0, 5))
    #    c.init_frustum(
    #        units.deg(45), 640 / 480, 0.005, 50
    #    )
    #    tree = Tree(62)
    #    import time
    #    start = time.time()
    #    all = find_nodes(tree, pos, c.frustum)
    #    print("%.2f ms" % ((time.time() - start) * 1000.0), len(all))
    #    start = time.time()
    #    tiles = {}
    #    min_x = 0
    #    max_x = 0
    #    min_z = 0
    #    max_z = 0
    #    for n in all:
    #        x, y, z = n.origin.x, n.origin.y, n.origin.z
    #        tile = tiles.setdefault((x, z, n.size), [0, 0])
    #        if y >= 0:
    #            tile[0] += 1
    #        else:
    #            tile[1] += 1
    #        if x < min_x: min_x = x
    #        if x + n.size > max_x: max_x = x + n.size
    #        if z < min_z: min_z = z
    #        if z + n.size > max_z: max_z = z + n.size
    #    max_i0 = float(max(i[0] for i in tiles.values())) or 1
    #    max_i1 = float(max(i[1] for i in tiles.values())) or 1
    #    print("prepare", len(tiles), "tiles: %.2f ms" % ((time.time() - start) * 1000.0))
    #    tile_w = 1000.0 / (max_x - min_x)
    #    tile_h = 1000.0 / (max_z - min_z)
    #    print(max_i0, max_i1, min_x, max_x, min_z, max_z, tile_w, tile_h)

    #    s = gl.Surface(1000, 1000)
    #    start = time.time()
    #    max_i3 = float(len(tiles))
    #    i3 = 0
    #    for (x, z, sz), (i0, i1) in sorted(tiles.items(), key = lambda t: t[0][2], reverse = True):
    #        i3 += 1
    #        r = gl.recti(
    #            int(float(x - min_x) * tile_w),
    #            int(float(z - min_z) * tile_h),
    #            int(tile_w * sz) or 1,
    #            int(tile_h * sz) or 1,
    #        )
    #        assert r.w > 0 and r.h > 0
    #        s.fill_rect(r, gl.col3f(i0 / max_i0, i1 / max_i1, i3 / max_i3))
    #    print("%.2f ms" % ((time.time() - start) * 1000.0))
    #    s.save_bmp(pathlib.Path('/tmp/lol.bmp'))

    #    #for y in set(n.origin.y for n in all):
    #    #    nodes = [
    #    #        n for n in all
    #    #        if n.origin.y == y
    #    #    ]
    #    #    if not nodes: continue
    #    #    s = gl.Surface(100, 100)
    #    #    for n in nodes:
    #    #        s.fill_rect(
    #    #            gl.recti(
    #    #                (n.origin.x - min_x) * tile_w,
    #    #                (n.origin.z - min_z) * tile_h,
    #    #                tile_w,
    #    #                tile_h
    #    #            ),
    #    #            gl.col3f("white")
    #    #        )
    #    #    s.save_bmp(pathlib.Path('/tmp/lol%s.bmp' % y))
