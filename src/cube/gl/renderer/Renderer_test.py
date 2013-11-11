
from . import create_renderer, delete_renderer
from cube.system.window import create_renderer_context,\
                               delete_renderer_context,\
                                WindowFlags
from cube.gl.renderer import Name

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
        delete_renderer(self.renderer)
        delete_renderer_context(self.context)

class _(TestCase, RendererSetup):
    def test_init(self):
        pass

