# -*- encoding: utf-8 -*-

from cubeapp import core

class Player(core.Player):
    velocity = .1
    def update(self, delta):
        if self.inputs.move_forward.held:
            self.camera.eye += self.velocity * self.camera.look
        elif self.inputs.move_backward.held:
            self.camera.eye -= self.velocity * self.camera.look

