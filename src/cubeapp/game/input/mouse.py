# -*- encoding: utf8 -*-

class MouseInput:
    def __init__(self):
        self.__xrel = self.__yrel = 0

    @property
    def xrel(self):
        ret = self.__xrel
        self.__xrel = 0
        return ret

    @xrel.setter
    def xrel(self, val):
        self.__xrel = val

    @property
    def yrel(self):
        ret = self.__yrel
        self.__yrel = 0
        return ret

    @yrel.setter
    def yrel(self, val):
        self.__yrel = val
