import os

from unittest import TestCase

from cube import gl
from cube.gl.renderer.Painter_test import PainterSetup, painter_test

class _(PainterSetup, TestCase):

    font_path = os.path.join(os.path.dirname(__file__), 'font_test.ttf')

    def setUp(self):
        super().setUp()

        x, y, w, h = (
            10, 10,
            180, 180
        )
        self.vb = self.renderer.new_vertex_buffer([
            gl.make_vba(
                gl.ContentKind.vertex,
                [
                    gl.vec3f(x, y, 0),
                    gl.vec3f(x, y + h, 0),
                    gl.vec3f(x + w, y + h, 0),
                    gl.vec3f(x + w, y, 0),
                ],
                gl.ContentHint.static_content
            ),
            gl.make_vba(
                gl.ContentKind.tex_coord0,
                [
                    gl.vec2f(0, 0),
                    gl.vec2f(0, 1),
                    gl.vec2f(1, 1),
                    gl.vec2f(1, 0),
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

    def test_simple(self):
        info = gl.font.get_infos(self.font_path)
        self.assertGreater(len(info), 0)
        self.assertIsNotNone(info[0])
        font_info = info[0]

        font = gl.Font(self.renderer, font_info, 48)
        self.assertIsNotNone(font)

        text = font.generate_text("This is a test")
        self.assertIsInstance(text, gl.VertexBuffer)
        self.assertIsInstance(font.texture, gl.Texture)


    @painter_test(gl.mode_2d)
    def test_render(self, painter):
        info = gl.font.get_infos(self.font_path)[0]
        font = gl.Font(self.renderer, info, 250)
        text = font.generate_text("a")

        material = gl.Material()
        material.add_texture(
            font.texture,
            gl.TextureType.ambient,
            gl.TextureMapping.uv,
            gl.StackOperation.add,
            gl.TextureMapMode.wrap,
            gl.BlendMode.basic
        )
        material.ambient = gl.Color3f("#311")
        with painter.bind([material.bindable(self.renderer)]):
            painter.draw_arrays(gl.DrawMode.quads, text, 0, 4)
