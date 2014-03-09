# -*- encoding: utf-8 -*-

from . import event
from . import input
from .controller import Controller

from cube import gl, log

def load(games_path, name, window):
    """Create a Game instance by loading a game called `name` in `games_path`.
    `window` instance is forwarded to the constructor of the Game class found
    in "${games_path}/${name}/game.py".
    """
    import sys
    old_python_path = sys.path
    sys.path = [games_path] + sys.path
    try:
        log.info("Loading game", name)
        game_module = __import__(name + '.game', fromlist=['game'])
    finally:
        sys.path = old_python_path
    return game_module.Game(window)

class Game():
    """Base class for every games.

    A game instance exposes the following attributes:
        * window: The operating system window instance.
        * renderer: The renderer.
        * world: An optional World instance.
        * view: An optional cube.gui.View object.
        * event_manager: The event manager (game related).
        * input_translator: Exposes bindings as event channels.
    """

    def __init__(self, window, bindings = {}, world = None, view = None):
        """Construct a game instance.

        # Positional parameters
            * window: An instance of cube.gui.Window.

        # Optional parameters
            * bindings: Bindings map (see cubeapp.game.input.Translator).
            * world: An instance of cubeapp.world.World.
            * view: An instance of cube.gui.View.

        # Bindings description
            Bindings constist of a dictionary where keys are simple string, and
            values are of the following form:
        """
        self.window = window
        self.renderer = window.renderer
        self.world = world
        self.view = view
        self.event_manager = event.Manager()
        self.input_translator = input.Translator(window, bindings)
        self.projection_matrix = gl.matrix.perspective(
            45, 1, 0.005, 300.0
        )
        self.window.inputs.on_quit.connect(self._on_quit)

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
        self.input_translator.poll()
        self.event_manager.poll(delta)
        if self.world is not None:
            self.world.update(delta, self.player, self.projection_matrix)

    def _on_quit(self):
        if self.world is not None:
            self.world.stop()
