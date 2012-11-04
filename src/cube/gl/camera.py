# -*- encoding: utf-8 -*-

from . import vector

class Camera:
    projection = None

    def __init__(self):
        self.eye = vector.Vector3f(0,0,0)
        self.look = vector.Vector3f(0, 0, -1)
        self.up = vector.Vector3f(0, 1, 0)
