# -*- encoding: utf-8 -*-

from .player import Player
from cube import gl

class Client:

    def __init__(self):
        self.player = Player()
        self.camera = gl.Camera()

