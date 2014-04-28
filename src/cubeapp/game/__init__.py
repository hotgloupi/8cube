# -*- encoding: utf-8 -*-

import os

from cube import gl, log, gui, scene

from . import event
from . import input
from . import entity

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

    return game_module.Game(
        window,
        directory = os.path.join(games_path, name)
    )

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

    def __init__(self,
                 window,
                 directory,
                 bindings = {},
                 scene = scene.Scene(),
                 view = gui.widgets.Viewport(),
                 event_manager = event.Manager()):
        """Construct a game instance.

        # Positional parameters
            * window: An instance of cube.gui.Window.

        # Optional parameters
            * bindings: Bindings map (see cubeapp.game.input.Translator).
            * scene: An instance of cube.scene.Scene.
            * view: An instance of cube.gui.View.

        # Bindings description
            Bindings constist of a dictionary where keys are simple string, and
            values are of the following form:
        """
        self.window = window
        self.renderer = window.renderer
        self.resource_manager = self.renderer.resource_manager
        resources_dir = os.path.join(directory, 'resources')
        if os.path.isdir(resources_dir):
            self.resource_manager.add_path(resources_dir)
        self.scene = scene
        self.view = view
        self.event_manager = event_manager
        self.entity_manager = entity.Manager(
            renderer = self.renderer,
            event_manager = self.event_manager,
            scene = self.scene,
        )
        self.input_translator = input.Translator(window, bindings)
        self.projection_matrix = gl.matrix.perspective(
            45, 1, 0.005, 300.0
        )
        self.window.inputs.on_quit.connect(self._on_quit)

    def update(self, delta):
        """
        @brief Update the game state according to the delta (in seconds).

        @note should be overridden.
        """
        self.input_translator.poll()
        self.event_manager.poll(delta)

    def _on_quit(self):
        pass
