# -*- encoding: utf-8 -*-

from cube import gl

class Chunk:

    __initialized = []

    def prepare(self, renderer):
        cls = Chunk
        if cls.__initialized:
            return
        cls.__initialized.append(True)
        print("INITIALIZE")
        r = renderer
        cls.__vb = r.new_vertex_buffer()
        w = 16
        h = 16
        cls.__vb.push_static_content(
            gl.ContentKind.vertex,
            list(
                gl.Vector3f(*v) for v in [
                    (0, 0, 0),
                    (w, 0, 0),
                    (w, 0, h),
                    (0, 0, h),
                ]
            )
        )
        cls.__vb.push_static_content(
            gl.ContentKind.color,
            [
                gl.Color3f('orange'),
                gl.Color3f('green'),
                gl.Color3f('gray'),
                gl.Color3f('blue'),
            ]
        )
        cls.__vb.finalize()
        cls.__indices = r.new_index_buffer()
        cls.__indices.push_static_content(
            gl.ContentKind.index,
            [0, 1, 2, 3]
        )
        cls.__indices.finalize()

        cls.__sp  = r.new_shader_program()
        fs = r.new_fragment_shader()
        fs.push_source("""
            void main(void)
            {
                gl_FragColor = gl_Color;
            }
        """)
        cls.__sp.push_shader(fs)
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
        cls.__sp.push_shader(vs)
        del vs
        cls.__sp.finalize()
        cls.__sp.parameter("cube_ModelViewProjectionMatrix")

    def render(self, pos, painter):
        painter.state.model = gl.matrix.translate(
            painter.state.model,
            gl.Vector3f(pos.x * 16, pos.y * 16, pos.z * 16)
        )
        cls = Chunk
        painter.bind(cls.__sp)
        painter.bind(cls.__vb)
        painter.draw_elements(gl.DrawMode.quads, cls.__indices, 0, 4)
