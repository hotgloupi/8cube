# -*- encoding: utf-8 -*-

from cube import gl
from cubeapp import core

class Game(core.Game):
    pass

class Player(core.Player):
    pass

def new_game(window, client, bindings, handlers):
    return Game(window, client, bindings, handlers)

def new_player(game, client):
    return Player(client)


def on_frame(game, painter):
    painter.state.view = gl.matrix.look_at(
        game.player.camera.eye,
        game.player.camera.look,
        game.player.camera.up
    )
