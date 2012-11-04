# -*- encoding: utf-8 -*-

from . import vector

class Camera:
    projection = None

    def __init__(self):
        self.position = vector.Vector3f(0,0,0)
        self.front = vector.Vector3f(0, 0, -1)
        self.up = vector.Vector3f(0, 1, 0)

    @property
    def right(self):
        return vector.cross(self.front, self.up)
