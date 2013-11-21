from .Painter_test import PainterSetup, painter_test

from cube import gl
from unittest import TestCase

class _(PainterSetup, TestCase):

    def setUp(self):
        super().setUp()

        self.vs = self.renderer.new_vertex_shader([
            """
            uniform mat4 cube_MVP;
            void main(void)
            {
                gl_Position = cube_MVP * gl_Vertex;
                gl_TexCoord[0] = gl_MultiTexCoord0;
            }
            """
        ])

        self.fs = self.renderer.new_fragment_shader([
            """
            uniform sampler2D sampler0;
            void main(void)
            {
                gl_FragColor = texture2D(sampler0, vec2(gl_TexCoord[0]));
            }
            """
        ])
        self.shader = self.renderer.new_shader_program([self.fs, self.vs])

        x, y, w, h = (
            0, 0,
            200, 200
        )
        self.vb = self.renderer.new_vertex_buffer([
            gl.make_vba(
                gl.ContentKind.vertex,
                [
                    gl.vec2f(x, y),
                    gl.vec2f(x + w, y),
                    gl.vec2f(x + w, y + h),
                    gl.vec2f(x, y + h)
                ],
                gl.ContentHint.static_content
            ),
            gl.make_vba(
                gl.ContentKind.tex_coord0,
                [
                    gl.vec2f(0, 0),
                    gl.vec2f(1, 0),
                    gl.vec2f(1, 1),
                    gl.vec2f(0, 1),
                ],
                gl.ContentHint.static_content
            )
        ])
        self.indices = self.renderer.new_index_buffer(
            gl.make_vba(
                gl.ContentKind.index,
                [ 0, 1, 2, 3],
                gl.ContentHint.static_content
            )
        )
        from os import path
        self.texture = self.renderer.new_texture(
            gl.Surface(path.join(path.dirname(__file__), "texture_test.bmp"))
        )


    @painter_test(gl.mode_2d)
    def test_simple_load(self, painter):
        with painter.bind([self.texture, self.shader, self.vb]):
            self.shader['cube_MVP'] = painter.state.mvp
            painter.draw_elements(gl.DrawMode.quads, self.indices, 0, 4)
