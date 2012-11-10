# -*- encoding: utf-8 -*-

from cube import gl

class Player:
    def __init__(self, client, inputs):
        self.camera = gl.Camera()
        self.client = client
        self.inputs = inputs


    @property
    def position(self):
        return self.camera.position

    def update(self, delta):
        """
        @brief Update the player state according to the elapsed time since last
               update.

        @note Should be overridden.
        """
        pass
