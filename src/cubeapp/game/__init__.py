# -*- encoding: utf-8 -*-

from . import event
from .event.manager import Manager as EventManager
from . import input

from cube import gl

def load(games_path, name, window, client):
    import sys
    old_python_path = sys.path
    sys.path = [games_path] + sys.path
    try:
        game_module = __import__(name + '.game', fromlist=['game'])
    finally:
        sys.path = old_python_path
    return game_module.Game(window, client)

class Game():
    """Base class for every games
    """

    def __init__(self, window, client, bindings, world):
        self.window = window
        self.renderer = window.renderer
        self.client = client
        self.world = world
        self.event_manager = EventManager()
        self.input_translator = input.Translator(window, bindings)
        self.projection_matrix = gl.matrix.perspective(
            45, 1, 0.005, 300.0
        )
        self.window.inputs.on_quit.connect(self._on_quit)

    @property
    def gui(self):
        """Game menu GUI"""
        raise Exception("Not implemented")

    @property
    def player(self):
        raise Exception("Not implemented")

    def on_resize(self, w, h):
        self.projection_matrix = gl.matrix.perspective(
            self.player.fov, w / h, 0.005, 300.0
        )

    def update(self, delta):
        """
        @brief Update the game state according to the delta (in seconds).

        @note should be overridden.
        """
        self.player.update(delta)
        self.world.update(delta, self.player, self.projection_matrix)

    def _on_quit(self):
        self.world.stop()
