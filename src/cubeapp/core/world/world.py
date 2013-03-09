# -*- encoding: utf-8 -*-

from cube import gl

from .storage import Storage
from .generator import Generator
from . import tree
from .chunk import Chunk

# World coordinate type
coord_type = gl.Vector3il

class World:
    def __init__(self, storage, generator, renderer):
        assert isinstance(storage, Storage)
        assert isinstance(generator, Generator)
        self.__storage = storage
        self.__generator = generator
        self.__renderer = renderer
        self.__tree = tree.Tree()
        Chunk.prepare(renderer)
        self.referential = gl.Vector3il()
        self.__nodes_to_render2 = []

    def update(self, delta, player, projection_matrix):
        self.__frustum = gl.frustum.Frustumil(45, 640.0 / 480.0,1,2)
        self.__frustum.update(
            player.world_position,
            gl.vector.normalize(player.camera.front),
            gl.vector.normalize(gl.vector.cross(player.camera.right, player.camera.front))
        )
        self.referential = player.world_position
        self.__nodes_to_render = []
        self.__checked = 0
        self.__tree.visit_nodes(self.__on_tree_node)
        print("Found", len(self.__nodes_to_render), "nodes",
              player.world_position, player.camera.front, player.camera.up, "check=",
             self.__checked)

    def __on_tree_node(self, node):
        #if self.__checked > 10000:
        #    print(".", end='')
        #    return tree.VisitAction.stop
        if node.level == 0:
            self.__nodes_to_render.append(
                (node.origin, self.get_chunk(node.origin))
            )
            return tree.VisitAction.continue_

        self.__checked += 1
        if len(self.__nodes_to_render) > 200:
            return tree.VisitAction.stop_and_clean

        center = gl.vec3il(
            node.origin.x + node.size / 2,
            node.origin.y + node.size / 2,
            node.origin.z + node.size / 2,
        )
        s = gl.Sphereil(center, int(node.size * 0.7071067811865476))
        if self.__frustum.intersect(s):
            if node.level>=200:
                print('found', node.level, node.origin, node.size, s)
            return tree.VisitAction.continue_

        if node.level>=600:
            print('NOT INTERSECT', node.level, node.origin, node.size, s)
        return tree.VisitAction.stop_and_clean

    def _fix(self):
        self.__nodes_to_render2 = self.__nodes_to_render[:]
    def get_chunk(self, pos):
        chunk = self.__storage.get_chunk(pos)
        if chunk is None:
            chunk = self.__generator.gen_chunk(pos)
            self.__storage.set_chunk(pos, chunk)
        return chunk

    def render(self, painter):
        with painter.bind([Chunk.sp, Chunk.vb]):
            for pos, chunk in self.__nodes_to_render2:
                try:
                    chunk.render(pos - self.referential, painter)
                except:
                    print(pos, self.referential, pos-self.referential)

