# -*- encoding: utf-8 -*-

from cubeapp import core
from cube import gl

class Player(core.Player):
    velocity = 10
    def update(self, delta):
        move = gl.Vector3f()
        if self.inputs.move_forward.held:
            move += self.camera.front
        elif self.inputs.move_backward.held:
            move -= self.camera.front

        if self.inputs.strafe_right.held:
            move += self.camera.right
        elif self.inputs.strafe_left.held:
            move -= self.camera.right

        if move.x or move.y or move.z:
            self.camera.position += self.velocity * delta * gl.vector.normalize(move)
