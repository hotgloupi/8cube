# -*- encoding: utf-8 -*-

from .chunk import Chunk
from cube import gl, units

class Generator:

    def gen_chunk(self, node):
        return Chunk(node)
