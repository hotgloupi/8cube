# -*- encoding: utf-8 -*-

from cube import gl
from .label import Label

class Button(Label):
    def __init__(self, text, **kwargs):
        kwargs.setdefault('tag', 'button')
        super(Button, self).__init__(text, **kwargs)

    def _prepare(self, renderer):
        super(Button, self)._prepare(renderer)
        x, y, w, h = (
            0, 0,
            self.size.x, self.size.y
        )
        attr = gl.renderer.make_vertex_buffer_attribute
        self.__vb = renderer.new_vertex_buffer([
            attr(
                gl.ContentKind.vertex,
                list(gl.Vector2f(*v) for v in [
                    (x, y),
                    (x + w, y),
                    (x + w, y + h),
                    (x, y + h)
                ]),
                gl.ContentHint.static_content
            ),
            attr(
                gl.ContentKind.color,
                [
                    gl.Color3f('#ccc'),
                    gl.Color3f('#ccc'),
                    gl.Color3f('#ccc'),
                    gl.Color3f('#ccd'),
                ],
                gl.ContentHint.static_content
            )
        ])
        self.__indices = renderer.new_index_buffer(
            attr(
                gl.ContentKind.index,
                [ 0, 1, 2, 3],
                gl.ContentHint.static_content
            )
        )

        self.__sp  = renderer.new_shader_program([
            renderer.new_fragment_shader(["""
            void main(void)
            {
                gl_FragColor = gl_Color;
            }
            """]),
            renderer.new_vertex_shader(["""
            uniform mat4 cube_ModelViewProjectionMatrix;
            void main(void)
            {
               gl_FrontColor = gl_Color;
               gl_Position = cube_ModelViewProjectionMatrix * gl_Vertex;
            }
            """]),
        ])

    def render(self, _):
        with self.renderer.begin(gl.renderer.mode_2d) as painter:
            painter.state.model = gl.matrix.translate(painter.state.model, self.position.x, self.position.y, .0)
            with painter.bind([self.__sp, self.__vb]):
                painter.draw_elements(gl.DrawMode.quads, self.__indices, 0, 4)
            super(Button, self).render(painter)
