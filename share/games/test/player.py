# -*- encoding: utf-8 -*-

from cubeapp import core
from cube import gl

class Player(core.Player):
    velocity = 10
    rot_velocity = .5

    def update(self, delta):
        xrel, yrel = self.inputs.mouse.xrel, self.inputs.mouse.yrel
        if xrel:
            self.camera.front = gl.vector.rotate_y(
                self.camera.front,
                -xrel * self.rot_velocity
            )
        if yrel:
            self.camera.front = gl.vector.rotate(
                self.camera.front,
                -yrel * self.rot_velocity,
                self.camera.right,
            )

        move = gl.Vector3f()
        if self.inputs.move_forward.held:
            move += self.camera.front
        if self.inputs.move_backward.held:
            move -= self.camera.front

        if self.inputs.strafe_right.held:
            move += self.camera.right
        if self.inputs.strafe_left.held:
            move -= self.camera.right

        if move.x or move.y or move.z:
            self.camera.position += self.velocity * delta * gl.vector.normalize(move)

