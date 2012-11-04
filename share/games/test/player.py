# -*- encoding: utf-8 -*-

from cubeapp import core

class Player(core.Player):
    velocity = 2
    def update(self, delta):
        if self.inputs.move_forward.held:
            self.camera.position += self.velocity * delta * self.camera.front
        elif self.inputs.move_backward.held:
            self.camera.position -= self.velocity * delta * self.camera.front

        if self.inputs.strafe_right.held:
            right = self.velocity * delta * self.camera.right
            self.camera.position += right
        elif self.inputs.strafe_left.held:
            right = self.velocity * delta * self.camera.right
            self.camera.position -= right
