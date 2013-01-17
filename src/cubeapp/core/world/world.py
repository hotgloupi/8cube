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

    def update(self, delta, player):
        self.referential = player.world_position
        self.__nodes_to_render = []
        self.__tree.visit_nodes(self.__on_tree_node)
        #print("Found", len(self.__nodes_to_render), "nodes")

    def __on_tree_node(self, node):
        if node.level == 0:
            self.__nodes_to_render.append(
                (node.origin, self.get_chunk(node.origin))
            )
        if node.contains(self.referential):
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
                chunk.render(pos - self.referential, painter)

