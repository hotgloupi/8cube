# -*- encoding: utf-8 -*-

from cube import gl
from .world import world, chunk

class Player:
    def __init__(self, client, inputs):
        self.camera = gl.Camera(
            gl.Vector3f(-0.1, 1, -0.1),
            gl.Vector3f(0, 0, -10),
            gl.Vector3f(0, 1, 0),
        )
        self.world_position = world.coord_type(0, 0, 0)
        self.client = client
        self.inputs = inputs
        self.update(0)

    @property
    def fov(self):
        return 45.0

    @property
    def position(self):
        return self.camera.position

    def update(self, delta):
        """
        @brief Update the player state according to the elapsed time since last
               update. base method set correct world position.

        @note Should be overridden.
        """
        world_decay = world.coord_type()
        if self.camera.position.x > chunk.Chunk.size:
            n = int(self.camera.position.x / chunk.Chunk.size)
            world_decay.x += n
        if self.camera.position.x < 0:
            n = -int(self.camera.position.x / chunk.Chunk.size) - 1
            world_decay.x += n
        if self.camera.position.y > chunk.Chunk.size:
            n = int(self.camera.position.y / chunk.Chunk.size)
            world_decay.y += n
        if self.camera.position.y < 0:
            n = -int(self.camera.position.y / chunk.Chunk.size) - 1
            world_decay.y += n
        if self.camera.position.z > chunk.Chunk.size:
            n = int(self.camera.position.z / chunk.Chunk.size)
            world_decay.z += n
        if self.camera.position.z < 0:
            n = -int(self.camera.position.z / chunk.Chunk.size) - 1
            world_decay.z += n

        self.camera.position.x -= world_decay.x * chunk.Chunk.size
        self.camera.position.y -= world_decay.y * chunk.Chunk.size
        self.camera.position.z -= world_decay.z * chunk.Chunk.size
        self.world_position += world_decay
        self.view_matrix = gl.matrix.look_at(
            self.camera.position,
            self.camera.position + self.camera.front,
            self.camera.up
        )

