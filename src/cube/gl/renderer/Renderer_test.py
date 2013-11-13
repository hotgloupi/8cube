
from . import create_renderer, delete_renderer
from cube.system.window import create_renderer_context,\
                               delete_renderer_context,\
                                WindowFlags
from cube.gl.renderer import Name, mode_2d, mode_3d, Painter

from unittest import TestCase

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

class _(RendererSetup, TestCase):
    def test_init(self):
        pass

    def test_begin(self):
        with self.renderer.begin(mode_2d) as p:
            self.assertIsInstance(p, Painter)
        with self.renderer.begin(mode_3d) as p:
            self.assertIsInstance(p, Painter)

