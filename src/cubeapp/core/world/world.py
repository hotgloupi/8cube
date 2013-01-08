# -*- encoding: utf-8 -*-

from cube import gl

from .storage import Storage
from .generator import Generator
from . import tree
from .chunk import Chunk

class World:
    def __init__(self, storage, generator, renderer):
        assert isinstance(storage, Storage)
        assert isinstance(generator, Generator)
        self.__storage = storage
        self.__generator = generator
        self.__renderer = renderer
        self.__tree = tree.Tree()
        Chunk.prepare(renderer)

    def update(self, delta, player):
        pos = self._chunk_coord(player.position)
        self.__player_pos = gl.Vector3il(pos.x, pos.y, pos.z)
        self.__nodes_to_render = []
        self.__tree.visit_nodes(self.__on_tree_node)
        #print("Found", len(self.__nodes_to_render), "nodes")

    def __on_tree_node(self, node):
        if node.level == 0:
            self.__nodes_to_render.append(
                (node.origin, self.get_chunk(node.origin))
            )
        if node.contains(self.__player_pos):
            return tree.VisitAction.continue_
        return tree.VisitAction.stop_and_clean

    def get_chunk(self, pos):
        chunk = self.__storage.get_chunk(pos)
        if chunk is None:
            chunk = self.__generator.gen_chunk(pos)
            self.__storage.set_chunk(pos, chunk)
        return chunk

    def render(self, painter):
        with painter.bind([Chunk.sp, Chunk.vb]):
            for pos, chunk in self.__nodes_to_render:
                chunk.render(pos, painter)

    @staticmethod
    def _chunk_coord(v):
        rounded = v / 16.0
        pos = gl.Vector3il(rounded.x, rounded.y, rounded.z)
        if v.x < 0:
            pos.x -= 1
        if v.y < 0:
            pos.y -= 1
        if v.z < 0:
            pos.z -= 1
        return pos

