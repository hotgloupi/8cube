import pathlib
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
        c = gl.Camera()
        pos = gl.vec3d(0, 20, 0)
        c.look_at(gl.vec3f(0.0, -1, 1))
        c.init_frustum(
            units.deg(45), 640 / 480, 0.005, 50
        )
        tree = Tree(62)
        import time
        start = time.time()
        all = find_nodes(tree, pos, c.frustum)
        print("%.2f ms" % ((time.time() - start) * 1000.0), len(all))
        start = time.time()
        tiles = {}
        min_x = 0
        max_x = 0
        min_z = 0
        max_z = 0
        for n in all:
            x, y, z = n.origin.x, n.origin.y, n.origin.z
            if y >= 0:
                tiles.setdefault((x, z), [0, 0])[0] += 1
            else:
                tiles.setdefault((x, z), [0, 0])[1] += 1
            if x < min_x: min_x = x
            if x > max_x: max_x = x
            if z < min_z: min_z = z
            if z > max_z: max_z = z
        max_i0 = float(max(i[0] for i in tiles.values())) or 1
        max_i1 = float(max(i[1] for i in tiles.values())) or 1
        print("prepare", len(tiles), "tiles: %.2f ms" % ((time.time() - start) * 1000.0))
        tile_w = int(1000 / (max_x - min_x))
        tile_h = int(1000 / (max_z - min_z))

        s = gl.Surface(1000, 1000)
        start = time.time()
        for (x, z), (i0, i1) in tiles.items():
            s.fill_rect(
                gl.recti(
                    (x - min_x) * tile_w,
                    (z - min_z) * tile_h,
                    tile_w,
                    tile_h
                ),
                gl.col3f(i0 / max_i0, i1 / max_i1, 0)
            )
        print("%.2f ms" % ((time.time() - start) * 1000.0))
        s.save_bmp(pathlib.Path('/tmp/lol.bmp'))

        #for y in set(n.origin.y for n in all):
        #    nodes = [
        #        n for n in all
        #        if n.origin.y == y
        #    ]
        #    if not nodes: continue
        #    s = gl.Surface(100, 100)
        #    for n in nodes:
        #        s.fill_rect(
        #            gl.recti(
        #                (n.origin.x - min_x) * tile_w,
        #                (n.origin.z - min_z) * tile_h,
        #                tile_w,
        #                tile_h
        #            ),
        #            gl.col3f("white")
        #        )
        #    s.save_bmp(pathlib.Path('/tmp/lol%s.bmp' % y))
