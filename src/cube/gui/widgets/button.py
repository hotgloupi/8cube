# -*- encoding: utf-8 -*-

from cube import gl
from .label import Label

class Button(Label):
    def __init__(self, text, **kwargs):
        kwargs.setdefault('tag', 'button')
        super(Button, self).__init__(text, **kwargs)

    def _prepare(self, renderer):
        super(Button, self)._prepare(renderer)
        self.__vb = renderer.new_vertex_buffer()
        x, y, w, h = (
            0, 0,
            self.size.x, self.size.y
        )
        self.__vb.push_static_content(
            gl.ContentKind.vertex,
            list(gl.Vector2f(*v) for v in [
                (x, y),
                (x + w, y),
                (x + w, y + h),
                (x, y + h)
            ])
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
        self.__indices = renderer.new_index_buffer()
        self.__indices.push_static_content(
            gl.ContentKind.index,
            [ 0, 1, 2, 3]
        )
        self.__indices.finalize()

        self.__sp  = renderer.new_shader_program()
        fs = renderer.new_fragment_shader()
        fs.push_source("""
            void main(void)
            {
                gl_FragColor = gl_Color;
            }
        """)
        self.__sp.push_shader(fs)
        del fs
        vs = renderer.new_vertex_shader()
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
        #with renderer.begin(gl.renderer.mode_2d) as painter:
        #    painter.bind(self.__sp)
        self.__sp.parameter("cube_ModelViewProjectionMatrix")
        #    renderer.viewport(0,0,640,480)

    def render(self, _):
        with self.renderer.begin(gl.renderer.mode_2d) as painter:
            painter.state.translate(self.position.x, self.position.y,0)
            painter.bind(self.__sp)
            #self.renderer.viewport(0,0,640,480)
            painter.bind(self.__vb)
            painter.draw_elements(gl.DrawMode.quads, self.__indices, 0, 4)
            super(Button, self).render(painter)
