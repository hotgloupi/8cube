# -*- encoding: utf-8 -*-

from .chunk import Chunk

class Generator:
    def gen_chunk(self, node):
        return Chunk(node)
