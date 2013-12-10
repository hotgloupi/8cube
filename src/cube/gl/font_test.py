import os

from unittest import TestCase

from cube import gl
from cube.gl.renderer.Painter_test import PainterSetup, painter_test

class _(PainterSetup, TestCase):

    font_path = os.path.join(os.path.dirname(__file__), 'font_test.ttf')
    takao_font_path = os.path.join(os.path.dirname(__file__), 'font_test_takao.ttf')

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

    @painter_test(gl.mode_2d)
    def test_big_string(self, painter):
        info = gl.font.get_infos(self.font_path)[0]
        font = gl.Font(self.renderer, info, 38)
        text = font.generate_text(
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "0123456789"
        )
        utf8 = font.generate_text("çàélöl")

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
        mat = material.bindable(self.renderer)
        with painter.bind([mat]):
            painter.draw_arrays(gl.DrawMode.quads, text, 0, 4 * (5))
        painter.push_state().translate(-100, 50, 0)
        with painter.bind([mat]):
            painter.draw_arrays(gl.DrawMode.quads, text, 4*5 , 4 * (5))
        painter.pop_state()
        painter.push_state().translate(0, 100, 0)
        with painter.bind([mat]):
            painter.draw_arrays(gl.DrawMode.quads, utf8, 0 , 40)
        painter.pop_state()

    @painter_test(gl.mode_2d)
    def test_opacity_channel(self, painter):
        info = gl.font.get_infos(self.takao_font_path)[0]
        font = gl.Font(self.renderer, info, 75)
        text = font.generate_text("東京")
        material = gl.Material()
        material.add_texture(
            font.texture,
            gl.TextureType.opacity,
            gl.TextureMapping.uv,
            gl.StackOperation.add,
            gl.TextureMapMode.wrap,
            gl.BlendMode.basic
        )
        material.ambient = gl.Color3f("#551")
        mat = material.bindable(self.renderer)
        painter.state.translate(0, 50, 0)
        with painter.bind([mat]):
            painter.draw_arrays(gl.DrawMode.quads, text, 0, 16)
