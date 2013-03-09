# -*- encoding: utf-8 -*-

from cubeapp import core
from cube import gl

class Player(core.Player):
    velocity = 50
    rot_velocity = .5

    def update(self, delta):
        xrel, yrel = self.inputs.mouse.xrel, self.inputs.mouse.yrel
        if xrel:
            self.camera.front = gl.vector.normalize(gl.vector.rotate(
                self.camera.front,
                -xrel * self.rot_velocity,
                self.camera.up
            ))
        if yrel and False:
            right = self.camera.right
            self.camera.front = gl.vector.rotate(
                self.camera.front,
                -yrel * self.rot_velocity,
                right,
            )
            self.camera.up = gl.vector.rotate(
                self.camera.up,
                -yrel * self.rot_velocity,
                right,
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
        super(Player, self).update(delta)

