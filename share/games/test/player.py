# -*- encoding: utf-8 -*-

from cubeapp import core

class Player(core.Player):
    velocity = 2
    def update(self, delta):
        if self.inputs.move_forward.held:
            self.camera.eye += self.velocity * delta * (self.camera.look - self.camera.eye)
        elif self.inputs.move_backward.held:
            self.camera.eye -= self.velocity * delta * (self.camera.look - self.camera.eye)

        if self.inputs.strafe_right.held:
            right = self.velocity * delta * self.camera.right_vector
            print(self.camera.look, self.camera.eye, self.camera.right_vector)
            self.camera.eye += right
            self.camera.look += right
        elif self.inputs.strafe_left.held:
            right = self.velocity * delta * self.camera.right_vector
            self.camera.eye -= right
            self.camera.look -= right

