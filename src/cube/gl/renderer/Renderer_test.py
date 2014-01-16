
from . import create_renderer, delete_renderer
from cube.system.window import create_renderer_context,\
                               delete_renderer_context,\
                                WindowFlags
from cube.gl.renderer import Name, mode_2d, mode_3d, Painter
from cube.gl.viewport import Viewport

from cube.test import Case

class RendererSetup:

    def setUp(self):
        self.name = Name.OpenGL
        self.context = create_renderer_context(
            640,
            480,
            WindowFlags.hidden,
            self.name
        )
        self.renderer = create_renderer(self.context)

    def tearDown(self):
        self.context = None
        self.renderer = None
        import gc
        gc.collect()

class _(RendererSetup, Case):
    def test_viewport(self):
        self.assertEqual(self.renderer.viewport.x, 0)
        self.assertEqual(self.renderer.viewport.y, 0)
        self.assertEqual(self.renderer.viewport.w, 640)
        self.assertEqual(self.renderer.viewport.h, 480)

        self.renderer.viewport.x = 42
        self.renderer.viewport.y = 42
        self.renderer.viewport.w = 42
        self.renderer.viewport.h = 42

        self.assertEqual(self.renderer.viewport.x, 0)
        self.assertEqual(self.renderer.viewport.y, 0)
        self.assertEqual(self.renderer.viewport.w, 640)
        self.assertEqual(self.renderer.viewport.h, 480)

    def test_begin(self):
        with self.renderer.begin(mode_2d) as p:
            self.assertIsInstance(p, Painter)
        with self.renderer.begin(mode_3d) as p:
            self.assertIsInstance(p, Painter)

