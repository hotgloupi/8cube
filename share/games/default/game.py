
import cubeapp
import cube

class GameView(cube.gui.widgets.Viewport):

    def __init__(self):
        super().__init__()
        self.cam = cube.gl.Camera()

    def render(self, painter):
        pass

class Game(cubeapp.game.Game):

    def __init__(self, **kw):
        super().__init__(view = GameView(), **kw)

