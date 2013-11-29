from .material import Material
from .renderer.Painter_test import PainterSetup, painter_test

from cube import gl

from unittest import TestCase

class _(PainterSetup, TestCase):


    def test_ctor(self):
        m = Material()
        self.assertEqual(m.name, "unamed")
        m = Material("Test")
        self.assertEqual(m.name, "Test")

    def test_properties(self):
        m = Material()
        m.name = "lol"
        self.assertEqual(m.name, "lol")
        black = gl.Color3f("black")
        pink = gl.Color3f("pink")

        self.assertEqual(m.ambient, black)
        m.ambient = pink
        self.assertEqual(m.ambient, pink)

        self.assertEqual(m.specular, black)
        m.specular = pink
        self.assertEqual(m.specular, pink)

        self.assertEqual(m.diffuse, black)
        m.diffuse = pink
        self.assertEqual(m.diffuse, pink)

        self.assertEqual(m.shininess, 0)
        m.shininess = 0.5
        self.assertEqual(m.shininess, 0.5)

        self.assertEqual(m.opacity, 1)
        m.opacity = 0.5
        self.assertEqual(m.opacity, 0.5)


    @painter_test(gl.mode_2d)
    def test_render_color0(self, painter):
        self.cube = gl.Mesh()
        self.cube.mode = gl.DrawMode.quads
        self.cube.append(gl.vec3f(20, 20, 0))
        self.cube.append(gl.vec3f(20, 180, 0))
        self.cube.append(gl.vec3f(180, 180, 0))
        self.cube.append(gl.vec3f(180, 20, 0))
        self.cube.kind = gl.ContentKind.color
        self.cube.append(gl.Color3f("bisque"))
        self.cube.append(gl.Color3f("brown"))
        self.cube.append(gl.Color3f("coral"))
        self.cube.append(gl.Color3f("gray"))
        material = Material()
        cube_view = self.cube.view(self.renderer)
        with painter.bind([material.bindable(self.renderer)]):
            painter.draw([cube_view])

