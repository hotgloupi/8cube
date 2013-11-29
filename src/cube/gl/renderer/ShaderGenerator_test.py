
from .Renderer_test import RendererSetup

from cube import gl

from unittest import TestCase

class _(RendererSetup, TestCase):

    def test_create_proxy(self):
        gen = self.renderer.generate_shader(gl.ShaderType.vertex)
        self.assertIsNotNone(gen)
        gen = self.renderer.generate_shader(gl.ShaderType.fragment)
        self.assertIsNotNone(gen)

    def test_vertex(self):
        gen = self.renderer.generate_shader(
            gl.ShaderType.vertex
        ).input(
            gl.ShaderParameterType.vec3,
            "cube_Vertex",
            gl.ContentKind.vertex
        ).input(
            gl.ShaderParameterType.vec3,
            "cube_VertexColor",
            gl.ContentKind.color
        ).output(
            gl.ShaderParameterType.vec3,
            "cube_Position",
            gl.ContentKind.vertex
        ).output(
            gl.ShaderParameterType.vec4,
            "cube_Color",
            gl.ContentKind.color
        )
        self.assertIsInstance(gen.source, str)

    def test_routine(self):
        class Routine(gl.ShaderRoutine):
            def source(self, lang):
                return "MY_CUSTOM_SOURCE_CODE"

        import gc
        gc.collect()
        self.assertEqual(len(gc.get_referrers(Routine)), 2)

        gen = self.renderer.generate_shader(gl.ShaderType.vertex).input(
            gl.ShaderParameterType.vec3,
            "cube_Vertex",
            gl.ContentKind.vertex
        ).routine(Routine, "TestingRoutine")

        src = gen.source
        self.assertIn("MY_CUSTOM_SOURCE_CODE", src)
        self.assertIn("TestingRoutine", src)

        del gen
        del src
        gc.collect()
        self.assertEqual(len(gc.get_referrers(Routine)), 2)
