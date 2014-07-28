import os

from cube.test import Case
from cube.gl.renderer.Painter_test import PainterSetup, painter_test
from cube import gl

class _(PainterSetup, Case):

    font_path = os.path.join(os.path.dirname(__file__), 'font', 'font_test.ttf')
    takao_font_path = os.path.join(os.path.dirname(__file__), 'font', 'font_test_takao.ttf')

    def setUp(self):
        super().setUp()
        info = gl.font.get_infos(self.font_path)[0]
        self.font = gl.Font(self.renderer, info, 48)

    def tearDown(self):
        self.font = None
        super().tearDown()

    @painter_test(gl.mode_2d)
    def test_simple(self, painter):
        text = gl.Text(self.font, "lol2")
        text.color = gl.Color3f("yellow")
        text.draw(painter)

