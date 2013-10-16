# -*- encoding: utf-8 -*-

from cubeapp import player, game
from cube import gl, units

class Player(player.Player):
    velocity = 200
    rot_velocity = .05


    def __init__(self, *args, **kw):
        super().__init__(*args, **kw)
        self.move_forward = game.event.Channel()
        self.move_backward = game.event.Channel()
        #xrel, yrel = self.inputs.mouse.xrel, self.inputs.mouse.yrel
        #if xrel or yrel:
        #    self.camera.rotate(
        #        units.deg(-xrel * self.rot_velocity),
        #        units.deg(yrel * self.rot_velocity)
        #    )

        self.event_manager.register(self.move_forward, self)
        self.event_manager.register(self.move_backward, self)

    def __call__(self, event, delta):
        move = gl.Vector3f()
        if event.channel == self.move_forward:
            move += self.camera.front
        elif event.channel == self.move_backward:
            move -= self.camera.front

        #if self.inputs.strafe_right.held:
        #    move += self.camera.right
        #if self.inputs.strafe_left.held:
        #    move -= self.camera.right

        if move.x or move.y or move.z:
            self.camera.move(self.velocity * delta * gl.vector.normalize(move))
        self._update_pos()

