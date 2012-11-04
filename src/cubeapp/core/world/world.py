# -*- encoding: utf-8 -*-

class World:
    def __init__(self, storage, generator):
        self.__storage = storage
        self.__generator = generator

    def update(self, delta, player):
        pass
