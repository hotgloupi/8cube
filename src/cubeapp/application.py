# -*- encoding: utf-8 -*-

import os
import time

import cube
from cube import gl

from . import game

class Application(cube.gui.Application):

    def __init__(self, game_directories = [], game_name = None):
        cube.debug("New application")
        game_dir = None
        for dir_ in game_directories:
            if os.path.isdir(os.path.join(dir_, game_name)):
                game_dir = dir_
                break
        if game_dir is None:
            raise Exception("Couldn't find a game named '%s'" % str(game_name))

        super().__init__(name = "8cube.io/%s" % game_name)
        self._game = game.load(
            game_dir,
            game_name,
            self.window,
        )
        self._game.event_manager.register(
            lambda ev, delta: self.shutdown(),
            self._game.shutdown_channel
        )
        #self.window.confine_mouse(True)

        #self._main_menu = self.viewport.emplace_child(
        #    cube.gui.widgets.VerticalLayout,
        #    renderer = self.window.renderer
        #)
        #self.__fps_label = self._main_menu.emplace_child(
        #    cube.gui.widgets.Label,
        #    "Frame time:", x = 0, y = 400, w = 400, h = 90,
        #    renderer = self.window.renderer
        #)
        #self.viewport.insert_child(self._game.view)

    def __del__(self):
        self._game = None

    def _run(self):
        self._running = True
        fps_target = 60
        frame_time_target = 1.0 / fps_target
        last_update = time.time()
        frame_time = frame_time_target
        while self._running is True:
            start = time.time()
            delta = start - last_update
            last_update = start
            self._game.update(delta)
            self.render()
            self.window.renderer.flush()
            self._window.swap_buffers()
            self._window.poll()
            self._game._poll(delta)
            frame_time = time.time() - start
            if frame_time < frame_time_target - 0.001:
                time.sleep(frame_time_target - frame_time)
                #print("%6.2f ms\n" % frame_time * 1000)

    def render(self):
        self.window.renderer.clear(
            gl.BufferBit.color | gl.BufferBit.depth
        )
        self._game.render()
        super().render()

    def _on_quit(self):
        self._game.event_manager.push(
            game.event.Event(self._game.escape_channel)
        )

    def shutdown(self):
        self._running = False
        super().shutdown()
        if self._game is not None:
            self._game.event_manager.clear()
            self._game.shutdown()
            self._game = None
