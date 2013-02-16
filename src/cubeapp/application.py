# -*- encoding: utf-8 -*-

import os
import time

import cube

from . import core

class Application(cube.gui.Application):

    def __init__(self, game_directories=[], game=None):
        super(Application, self).__init__(
            name="8cube",
        )
        cube.debug("New application")
        game_dir = None
        for dir_ in game_directories:
            if os.path.isdir(os.path.join(dir_, game)):
                game_dir = dir_
                break
        if game_dir is None:
            raise Exception("Couldn't find a game named '%s'" % str(game))

        self._client = core.Client()
        self._game = core.load_game(game_dir, game,
                                    self.window,
                                    self._client)
        self.window.confine_mouse(True)
        self.__prepare()

    def __prepare(self):

        self._main_menu = cube.gui.widgets.VerticalLayout(
            renderer=self.window.renderer
        ).add_child(
            cube.gui.widgets.Button("New game", x=10, y=100, w=400, h=90)
        )
        self._game_menu = self._game.gui
        self.viewport.add_child(
            self._game_menu
        )

    def run(self):
        self._running = True
        fps_target = 50
        frame_time_target = 1.0 / fps_target
        frame_counter_start_time = time.time()
        nb_frame = 0
        last_update = time.time()
        while self._running is True:
            start = time.time()
            self._window.poll()
            self._update(time.time() - last_update)
            last_update = time.time()
            frame_time = time.time() - start
            #if frame_time < frame_time_target:
            self.render()
            frame_time = time.time() - start
            if frame_time < frame_time_target:
                time.sleep(frame_time_target - frame_time)
            nb_frame += 1
            if time.time() - frame_counter_start_time  > 1:
                print("FPS:", nb_frame / (time.time() - frame_counter_start_time))
                frame_counter_start_time = time.time()
                nb_frame = 0

    def _update(self, delta):
        self._game.update(delta)

    def _on_quit(self):
        self.stop()

