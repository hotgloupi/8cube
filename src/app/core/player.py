# -*- encoding: utf-8 -*-

from cube import gl

class Player:
    def __init__(self, client):
        self.camera = gl.Camera()
        self.client = client

