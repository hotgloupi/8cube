# -*- encoding: utf-8 -*-


class Storage:

    def __init__(self):
        self.__chunks = {}

    def get_chunk(self, node):
        assert node is not None
        return self.__chunks.get(node)

    def set_chunk(self, node, chunk):
        assert node is not None
        assert chunk is not None
        assert self.__chunks.get(node) is None
        self.__chunks[node] = chunk
