# -*- encoding: utf-8 -*-

from cube import gl

class Game():

    def __init__(self, window, client):
        self.window = window
        self.renderer = window.renderer
        self.client = client
        self._prepare()

    @property
    def player(self):
        return self.client.player

    def _prepare(self):
        assert self.renderer is not None
        r = self.renderer
        self.__vb = r.new_vertex_buffer()
        w = 5
        h = 5
        self.__vb.push_static_content(
            gl.ContentKind.vertex,
            list(
                gl.Vector3f(*v) for v in [
                    (0, 0, -10),
                    (w, 0, -10),
                    (w, h, -10),
                    (0, h, -10),
                ]
            )
        )
        self.__vb.push_static_content(
            gl.ContentKind.color,
            [
                gl.Color3f('orange'),
                gl.Color3f('green'),
                gl.Color3f('gray'),
                gl.Color3f('blue'),
            ]
        )
        self.__vb.finalize()
        self.__indices = r.new_index_buffer()
        self.__indices.push_static_content(
            gl.ContentKind.index,
            [ 0, 1, 2, 3]
        )
        self.__indices.finalize()

        self.__sp  = r.new_shader_program()
        fs = r.new_fragment_shader()
        fs.push_source("""
            void main(void)
            {
                gl_FragColor = gl_Color;
            }
        """)
        self.__sp.push_shader(fs)
        del fs
        vs = r.new_vertex_shader()
        vs.push_source("""
            uniform mat4 cube_ModelViewProjectionMatrix;
            void main(void)
            {
               gl_FrontColor = gl_Color;
               gl_Position = cube_ModelViewProjectionMatrix * gl_Vertex;
            }
        """)
        self.__sp.push_shader(vs)
        del vs
        self.__sp.finalize()
        self.__sp.parameter("cube_ModelViewProjectionMatrix")
        self.__projection_matrix = gl.matrix.perspective(
            90, self.window.size.x / self.window.size.y, 0.05, 300.0
        )

    def on_resize(self, w, h):
        self.__projection_matrix = gl.matrix.perspective(
            90, w / h, 0.05, 300.0
        )

    def render(self):
        with self.renderer.begin(gl.mode_2d) as painter:
            painter.state.projection = self.__projection_matrix
            painter.state.view = gl.matrix.look_at(
                self.client.camera.eye,
                self.client.camera.look,
                self.client.camera.up
            )
            painter.bind(self.__sp)
            painter.bind(self.__vb)
            painter.draw_elements(gl.DrawMode.quads, self.__indices, 0, 4)
