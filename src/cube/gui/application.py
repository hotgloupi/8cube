
import cube.application
from cube.system.window import Window

class Application(cube.application.Application):
    def __init__(self):
        self._window = Window("8cube", 640, 480)
        self._window.poll()
