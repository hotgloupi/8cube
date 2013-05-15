# -*- encoding: utf-8 -*-

class Kind:
    mousemove = 1
    mouseenter = 2
    mouseleave = 3
    mouseclick = 4
    keydown = 5
    keyup = 6

class Event:
    def __init__(self, kind):
        self.__kind = kind
        self.__stopped = False

    @property
    def kind(self):
        return self.__kind

    @property
    def stopped(self):
        return self.__stopped

    def stop(self):
        self.__stopped = True


