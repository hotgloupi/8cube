# -*- encoding: utf-8 -*-

import time
import cube

vertex_shader = """
uniform mat4 cube_ModelViewProjectionMatrix;
void main()
{
    gl_FrontColor = gl_Color;
    gl_Position = cube_ModelViewProjectionMatrix * gl_Vertex;
}
"""

fragment_shader = """
void main()
{
    gl_FragColor = gl_Color;
}

"""

class App:
    def __init__(self):
        self.window = cube.system.Window("Testing", 640, 480)
        self.window.inputs.on_quit.connect(self._on_quit)
        self.window.inputs.on_resize.connect(self._on_resize)
        self.running = False
        self._prepare()
        self._on_resize(640, 480)


    def _prepare(self):
        from cube.gl.renderer import make_vertex_buffer_attribute as vb_attr, \
                                     ContentHint, ContentKind
        from cube.gl import Color3f, Vector3f

        r = self.window.renderer
        w = h = 1
        self.vb = r.new_vertex_buffer([
            vb_attr(
                ContentKind.vertex,
                list(
                    Vector3f(*v) for v in [
                        (w, h, 0),
                        (-w, h, 0),
                        (-w, -h, 0),
                        (w, -h, 0),
                    ]
                ),
                ContentHint.static_content
            ),
            vb_attr(
                ContentKind.color,
                list(Color3f(v) for v in ('red', 'orange', 'green', 'blue')),
                ContentHint.static_content
            )
        ])

        self.ib = r.new_index_buffer(
            vb_attr(
                ContentKind.index,
                [0, 1, 2, 3],
                ContentHint.static_content
            )
        )

        self.shader = r.new_shader_program([
            r.new_vertex_shader([vertex_shader]),
            r.new_fragment_shader([fragment_shader]),
        ])
        self.shader.parameter("cube_ModelViewProjectionMatrix")

    def _on_resize(self, w, h):
        print("RESIZE")
        self.projection_matrix = cube.gl.matrix.perspective(
            45, w / h, 0.005, 300.0
        )



    def _on_quit(self):
        self.running = False

    def run(self):
        self.running = True
        while self.running:
            self.window.poll()
            self.render()
            time.sleep(0.05)

    def render(self):
        print('#' * 80, time.time())
        r = self.window.renderer
        r.clear(cube.gl.BufferBit.color | cube.gl.BufferBit.depth)
        with r.begin(cube.gl.mode_2d) as painter:
            painter.state.projection = self.projection_matrix
            painter.state.view = cube.gl.matrix.look_at(
                cube.gl.Vector3f(0, 0, -5),
                cube.gl.Vector3f(0, 0, 1),
                cube.gl.Vector3f(0, 1, 0),
            )
            painter.bind(self.vb)
            painter.bind(self.shader)
            painter.draw_elements(
                cube.gl.DrawMode.quads,
                self.ib,
                0, 4
            )
        r.swap_buffers()

if __name__ == '__main__':
    App().run()
