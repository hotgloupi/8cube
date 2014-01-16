
from .Renderer_test import RendererSetup
from cube.gl.renderer import mode_2d

from cube.test import Case

class _(RendererSetup, Case):

    def setUp(self):
        super().setUp()
        self.target = self.renderer.context.new_render_target()

    def test_bind(self):
        with self.renderer.begin(mode_2d) as painter:
            with painter.bind([self.target]):
                pass
