
import time

import cube

class Application(cube.Application):
    def __init__(self):
        self._window = cube.gui.RootWindow("8cube", 640, 480)
        events = [
            'idle', 'expose', 'quit',
        ]
        self._handlers = {}
        for event in events:
            connection = self._window.connect(event, getattr(self, '_on_' + event))
            self._handlers[event] = {'connection': connection}

    def __del__(self):
        self.shutdown()

    def run(self):
        frame_time_target = 50 / 1000
        self._running = True
        while self._running is True:
            start = time.time()
            self._window.poll()
            frame_time = time.time() - start
            if frame_time < frame_time_target:
                self._window.render()
            frame_time = time.time() - start
            if frame_time < frame_time_target:
                time.sleep(frame_time_target - frame_time)

    def shutdown(self):
        print("shutting down")
        for ev, hdlr in self._handlers.items():
            hdlr['connection'].disconnect()
        self._handlers = {}
        self._window = None


    def _on_idle(self):
        pass

    def _on_expose(self, width, height):
        self._window.renderer.swap_buffers()
        #renderer.swap_buffers();
        print("on expose", self, width, height)

    def _on_quit(self):
        self._running = False
