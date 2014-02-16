# -*- encoding: utf-8 -*-

import os
import time

import cube

from . import game
from .client import Client

class Application(cube.gui.Application):

    def __init__(self, game_directories = [], game_name = None):
        cube.debug("New application")
        game_dir = None
        for dir_ in game_directories:
            if os.path.isdir(os.path.join(dir_, game_name)):
                game_dir = dir_
                break
        if game_dir is None:
            raise Exception("Couldn't find a game named '%s'" % str(game))

        super().__init__(name = "cubeapp - %s" % game_name)
        self._client = Client()
        self._game = game.load(
            game_dir,
            game_name,
            self.window,
            self._client
        )
        #self.window.confine_mouse(True)

        self._main_menu = self.viewport.emplace_child(
            cube.gui.widgets.VerticalLayout,
            renderer = self.window.renderer
        )
        self.__fps_label = self._main_menu.emplace_child(
            cube.gui.widgets.Label,
            "Frame time:", x = 0, y = 400, w = 400, h = 90,
            renderer = self.window.renderer
        )
        self.viewport.insert_child(self._game.view)

    def run(self):
        self._running = True
        fps_target = 60
        frame_time_target = 1.0 / fps_target
        last_update = time.time()
        while self._running is True:
            start = time.time()
            self._window.poll()
            self._window.renderer.flush()
            self._update(time.time() - last_update)
            last_update = time.time()
            frame_time = time.time() - start
            #if frame_time < frame_time_target:
            self.render()
            frame_time = time.time() - start
            if frame_time < frame_time_target:
                time.sleep(frame_time_target - frame_time)
            self.__fps_label.text = "Frame time: %6.2f ms (targetting %6.2f ms)" % (
                frame_time * 1000,
                frame_time_target * 1000,
            )

    def _update(self, delta):
        self._game.update(delta)

    def _on_quit(self):
        self.stop()

