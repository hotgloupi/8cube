
import time

import cube

class Application(cube.Application):
    def __init__(self, name="Cube app", width=640, height=480):
        self._window = cube.gui.RootWindow(name, width, height)
        events = [
            'idle', 'quit',
        ]
        self._handlers = {}
        for event in events:
            connection = getattr(
                self._window.inputs, 'on_' + event
            ).connect(getattr(self, '_on_' + event))
            self._handlers[event] = {'connection': connection}

    @property
    def viewport(self):
        return self._window.root_widget

    def __del__(self):
        self.shutdown()

    def run(self):
        self._running = True
        frame_time_target = 50 / 1000
        while self._running is True:
            start = time.time()
            self._window.poll()
            frame_time = time.time() - start
            if frame_time < frame_time_target:
                self.render()
            frame_time = time.time() - start
            if frame_time < frame_time_target:
                time.sleep(frame_time_target - frame_time)

    def render(self):
        with self._window.renderer.begin(cube.gl.renderer.mode_2d) as painter:
            self._window.render(painter)

    def shutdown(self):
        print("shutting down")
        for ev, hdlr in self._handlers.items():
            hdlr['connection'].disconnect()
        self._handlers = {}
        self._window = None


    def _on_idle(self):
        pass

    def _on_quit(self):
        self._running = False
