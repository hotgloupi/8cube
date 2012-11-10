# -*- encoding: utf-8 -*-

from cube import gl

from .storage import Storage
from .generator import Generator

class World:
    def __init__(self, storage, generator, renderer):
        assert isinstance(storage, Storage)
        assert isinstance(generator, Generator)
        self.__storage = storage
        self.__generator = generator
        self.__renderer = renderer
        self.__xxx_chunks = {}

    def update(self, delta, player):
        pos = self._chunk_coord(player.position)
        self.__xxx_chunks[(pos.x, pos.y, pos.z)] = self.get_chunk(pos)

    def get_chunk(self, pos):
        chunk = self.__storage.get_chunk(pos)
        if chunk is None:
            chunk = self.__generator.gen_chunk(pos)
            chunk.prepare(self.__renderer)
            self.__storage.set_chunk(pos, chunk)
        return chunk

    def render(self, renderer):
        for pos, chunk in self.__xxx_chunks.items():
            with renderer.begin(gl.mode_3d) as painter:
                chunk.render(gl.Vector3i(*pos), painter)

    @staticmethod
    def _chunk_coord(v):
        rounded = v / 16.0
        pos = gl.Vector3i(rounded.x, rounded.y, rounded.z)
        if v.x < 0:
            pos.x -= 1
        if v.y < 0:
            pos.y -= 1
        if v.z < 0:
            pos.z -= 1
        return pos

