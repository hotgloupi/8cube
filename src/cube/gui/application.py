
import cube.application
from cube.system.window import Window

class Application(cube.application.Application):
    def __init__(self):
        self._window = Window("8cube", 640, 480)
        print("self =", self)
        events = [
            'idle', 'expose',
        ]
        self._handlers = {}
        for event in events:
            connection = self.connect(event, getattr(self, '_on_' + event))
            self._handlers[event] = {'connection': connection}
        self._window.poll()

    def __del__(self):
        self.shutdown()

    def shutdown(self):
        print("shutting down")
        for ev, hdlr in self._handlers.items():
            hdlr['connection'].disconnect()
        self._handlers = {}
        self._window = None


    def connect(self, event, callback):
        return getattr(self._window, 'connect_' + event)(callback)


    def _on_idle(self):
        print("On idle!", self)

    def _on_expose(self, width, height):
        renderer = self._window.renderer()
        print("on expose", self, width, height)
