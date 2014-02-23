
import time

import cube

class Application(cube.Application):
    def __init__(self, name="Cube app", width=800, height=480):
        events = [
            'idle', 'quit',
        ]
        self._handlers = {}
        self._window = cube.gui.Window(name, width, height)
        for event in events:
            connection = getattr(
                self._window.inputs, 'on_' + event
            ).connect(getattr(self, '_on_' + event))
            self._handlers[event] = {'connection': connection}

    @property
    def viewport(self):
        return self._window.root_widget

    @property
    def window(self):
        return self._window

    def __del__(self):
        self.shutdown()

    def run(self):
        self._running = True
        fps_target = 50
        frame_time_target = 1.0 / fps_target
        frame_counter_start_time = time.time()
        nb_frame = 0
        while self._running is True:
            start = time.time()
            self._window.poll()
            frame_time = time.time() - start
            if frame_time < frame_time_target:
                self.render()
            frame_time = time.time() - start
            if frame_time < frame_time_target:
                time.sleep(frame_time_target - frame_time)
            nb_frame += 1
            if time.time() - frame_counter_start_time  > 1:
                print("FPS:", nb_frame / (time.time() - frame_counter_start_time))
                frame_counter_start_time = time.time()
                nb_frame = 0

    def render(self):
        with self._window.renderer.begin(cube.gl.renderer.mode_2d) as painter:
            self._window.render(painter)

    def stop(self):
        self._running = False

    def shutdown(self):
        cube.debug("shutting down")
        for ev, hdlr in self._handlers.items():
            hdlr['connection'].disconnect()
        self._handlers = {}


    def _on_idle(self):
        pass

    def _on_quit(self):
        self._running = False
