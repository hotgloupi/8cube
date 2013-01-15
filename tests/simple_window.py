# -*- encoding: utf-8 -*-

import time
import cube

vertex_shader = """
uniform mat4 cube_ModelViewProjectionMatrix;
void main()
{
    gl_FrontColor = gl_Color;
    gl_Position = cube_ModelViewProjectionMatrix * gl_Vertex;
    gl_TexCoord[0] = gl_MultiTexCoord0;
}
"""

fragment_shader = """
uniform sampler2D sampler0;
void main()
{
    vec4 font = texture2D(sampler0, vec2(gl_TexCoord[0]));
    gl_FragColor =font;
}

"""
text_fragment_shader = """
uniform sampler2D sampler0;
void main(void)
{
   gl_FragColor = texture2D(sampler0, vec2(gl_TexCoord[0]));
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
        from cube.gl import Color3f, Vector3f, Vector2f

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
                ContentKind.tex_coord0,
                list(Vector3f(*v) for v in [
                    (0, -1, 0),
                    (1, -1, 0),
                    (1, 0, 0),
                    (0, 0, 0),
                ]),
                ContentHint.static_content
            ),
            #vb_attr(
            #    ContentKind.color,
            #    list(Color3f(v) for v in ('red', 'orange', 'green', 'blue')),
            #    ContentHint.static_content
            #)
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
        #self.shader.parameter("cube_ModelViewProjectionMatrix")

        infos = None
        for name in ['monaco', 'monospace', 'mono']:
            infos = cube.gui.FontManager.search(name)
            if len(infos):
                print("Found font", name, infos[0])
                break
        if not infos:
            raise Exception("Font not found")
        self.font = cube.gl.Font(r, infos[0], 48)
        self.text = cube.gl.Text(self.font, "abcdefghijklmnopqrstuvwxyz")
        #self.text_vb = self.font.generate_text("Hello")
        self.text_shader = r.new_shader_program([
            r.new_vertex_shader([vertex_shader]),
            r.new_fragment_shader([text_fragment_shader]),
        ])
        tex = self.font.texture
        param = self.shader.parameter("sampler0")
        param.set(tex)

    def _on_resize(self, w, h):
        self.projection_matrix = cube.gl.matrix.perspective(
            45, h / w, 0.005, 300.0
        )
        self.window.renderer.clear(cube.gl.BufferBit.color | cube.gl.BufferBit.depth)
        self.window.renderer.viewport(0, 0, w, h)

    def _on_quit(self):
        self.running = False

    def run(self):
        self.running = True
        while self.running:
            self.window.poll()
            self.render()
            time.sleep(0.05)

    def render(self):
        r = self.window.renderer
        r.clear(cube.gl.BufferBit.color | cube.gl.BufferBit.depth)

        with r.begin(cube.gl.mode_3d) as painter:
            painter.state.projection = self.projection_matrix
            painter.state.view = cube.gl.matrix.look_at(
                cube.gl.Vector3f(0, 0, -5),
                cube.gl.Vector3f(0, 0, 1),
                cube.gl.Vector3f(0, 1, 0),
            )
            with painter.bind([self.vb, self.shader, self.font.texture]):
                painter.draw_elements(
                    cube.gl.DrawMode.quads,
                    self.ib,
                    0, 4
                )
        with r.begin(cube.gl.mode_3d) as painter:
            painter.state.model = cube.gl.matrix.translate(
                cube.gl.matrix.scale(painter.state.model, .01, -.01, .01),
                -70, 0, 0
            )
            painter.state.projection = self.projection_matrix
            painter.state.view = cube.gl.matrix.look_at(
                cube.gl.Vector3f(0, 0, 10),
                cube.gl.Vector3f(0, 0, -1),
                cube.gl.Vector3f(0, 1, 0),
            )
            with painter.bind([self.text_shader]):
                painter.draw(self.text, self.text_shader.parameter('sampler0'))

        r.swap_buffers()

if __name__ == '__main__':
    cube.gui.FontManager.populate()
    App().run()
