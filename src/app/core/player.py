# -*- encoding: utf-8 -*-

from cube import gl

class Player:
    def __init__(self, client, inputs):
        self.camera = gl.Camera()
        self.client = client
        self.inputs = inputs

