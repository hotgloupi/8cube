# -*- encoding: utf-8 -*-

from cubeapp import core
from cube import gl, units

class Player(core.Player):
    velocity = 200
    rot_velocity = .5

    def update(self, delta):
        xrel, yrel = self.inputs.mouse.xrel, self.inputs.mouse.yrel
        if xrel or yrel:
            self.camera.rotate(
                units.deg(-xrel * self.rot_velocity),
                units.deg(yrel * self.rot_velocity)
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
            self.camera.move(self.velocity * delta * gl.vector.normalize(move))
        super(Player, self).update(delta)

