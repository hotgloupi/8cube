# -*- encoding: utf-8 -*-


class Storage:

    def __init__(self):
        self.__chunks = {}

    def get_chunk(self, pos):
        assert pos is not None
        return self.__chunks.get(
            (pos.x, pos.y, pos.z)
        )

    def set_chunk(self, pos, chunk):
        assert pos is not None
        assert chunk is not None
        assert self.__chunks.get((pos.x, pos.y, pos.z)) is None
        self.__chunks[(pos.x, pos.y, pos.z)] = chunk
