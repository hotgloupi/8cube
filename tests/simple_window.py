# -*- encoding: utf-8 -*-

import time
import cube

class App:
    def __init__(self):
        self.window = cube.system.Window("Testing", 640, 480)
        self.window.inputs.on_quit.connect(self._quit)
        self.running = False

    def _quit(self):
        self.running = False

    def run(self):
        self.running = True
        while self.running:
            self.window.poll()
            time.sleep(0.05)

if __name__ == '__main__':
    App().run()
