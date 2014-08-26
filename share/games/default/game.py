
import cubeapp
import cube
from cube import gl, units

DOCUMENT = """
<rml>
    <head>
    <style>
    body {
        font-family: FreeMono;
        font-size: 30pt;
        color: white;
    }
    </style>
    </head>
    <body>
        <p>8cube framework</p>
    </body>
</rml>
"""

class Game(cubeapp.game.Game):

    def __init__(self, **kw):
        super().__init__(**kw)
        self.window.add_font(self.directory / 'FreeMono.ttf')
        self.window.load_document(DOCUMENT)
        self.cube =  cube.gl.Cube3f(cube.gl.vec3f(), 1)
        self.view = self.cube.drawable(self.renderer)
        material = gl.Material()
        material.ambient = gl.Color3f("red")
        self.material_view = material.bindable(self.renderer)

    def render(self):
        with self.renderer.begin(gl.mode_3d) as painter:
            painter.state.look_at(
                gl.vec3f(0,0,-10), gl.vec3f(0, 0, 0), gl.vec3f(0, 1, 0)
            )
            painter.state.perspective(
                units.deg(45), self.window.width / self.window.height, 0.005, 300.0
            )
            with painter.bind([self.material_view]):
                painter.draw([self.view])


