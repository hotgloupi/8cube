# -*- encoding: utf8 -*-

from unittest import TestCase

from cube.exception import Exception as CubeException

from .constants import ShaderType
from .Renderer_test import RendererSetup


class ShaderSetup(RendererSetup):

    def setUp(self):
        super().setUp()
        self.fs = self.renderer.new_fragment_shader(["""
            void main(void)
            {
                gl_FragColor = gl_Color;
            }
        """])
        self.vs = self.renderer.new_vertex_shader([
            """
            uniform mat4 cube_MVP;
            void main(void)
            {
               gl_FrontColor = gl_Color;
               gl_Position = cube_MVP * gl_Vertex;
            }
            """
        ])

class _(ShaderSetup, TestCase):

    def test_init(self):
        pass

    def test_init_empty_list(self):
        try:
            self.renderer.new_shader(ShaderType.vertex, [])
        except CubeException as e:
            self.assertEqual(str(e), "Source list is empty")
        else:
            self.fail("Shouldn't be able to create a shader from an empty "
                      "source list")

    def test_init_invalid_source(self):
        try:
            self.renderer.new_shader(ShaderType.fragment, [":LOL:"])
        except CubeException as e:
            self.assertTrue(str(e).startswith("Cannot compile shader:"))
        else:
            self.fail("Shouldn't be able to create a shader from an invalid "
                      "source")
