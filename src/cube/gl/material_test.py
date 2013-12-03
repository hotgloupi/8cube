from .material import Material, StackOperation
from .renderer.Painter_test import PainterSetup, painter_test
from cube.units import angle

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

    def setUp(self):
        super().setUp()
        self.__cube = None

    @property
    def cube(self):
        if self.__cube is not None:
            return self.__cube
        self.__cube = gl.Mesh()

        vertices = [
            (-.5, -.5,  .5),
            (-.5,  .5,  .5),
            (.5,  .5,  .5),
            (.5, -.5,  .5),
            (-.5, -.5, -.5),
            (-.5,  .5, -.5),
            (.5,  .5, -.5),
            (.5, -.5, -.5),
        ]
        colors = [
            (0, 0, 1),
            (1, 0, 0),
            (0, 1, 0),
            (1, 1, 0),
            (1, 1, 1),
            (1, 0, 0),
            (1, 0, 1),
            (0, 0, 1),
        ]
        indices = [
            0,2,1,  0,3,2,
            4,3,0,  4,7,3,
            4,1,5,  4,0,1,
            3,6,2,  3,7,6,
            1,6,5,  1,2,6,
            7,5,6,  7,4,5
        ]
        self.__cube.mode = gl.DrawMode.triangles

        for i in indices:
            self.__cube.kind = gl.ContentKind.vertex
            self.__cube.append(gl.vec3f(*vertices[i]))
            self.__cube.kind = gl.ContentKind.color
            self.__cube.append(gl.Color3f(*colors[i]))

        return self.__cube

    deg = 0

    @painter_test(gl.mode_2d)
    def test_render_color0(self, painter):
        material = Material()
        material.add_color(
            gl.ShaderParameterType.vec3,
            StackOperation.add
        )
        painter.state.translate(100, 100, 0)
        painter.state.scale(100, 100, 0)
        painter.state.rotate(angle.deg(20), gl.vec3f(1, 1, 0))
        cube_view = self.cube.view(self.renderer)
        with painter.bind([material.bindable(self.renderer)]):
            painter.draw([cube_view])

    @painter_test(gl.mode_3d)
    def test_cube(self, painter):
        material = Material()
        material.add_color(
            gl.ShaderParameterType.vec3,
            StackOperation.add
        )
        cube_view = self.cube.view(self.renderer)
        painter.state.look_at(
            gl.vec3f(0, .8, -3), gl.vec3f(0, 0, 0), gl.vec3f(0, 1, 0)
        )
        painter.state.perspective(
            35, 200 / 200, 0.005, 30.0
        )
        painter.state.rotate(angle.deg(32), gl.vec3f(0, 1, 0))
        with painter.bind([material.bindable(self.renderer)]):
            painter.draw([cube_view])

  #  @painter_test(gl.mode_3d)
  #  def test_cube_light(self, painter):
  #      material = Material()
  #      cube_view = self.cube.view(self.renderer)
  #      painter.state.look_at(
  #          gl.vec3f(0, .8, -3), gl.vec3f(0, 0, 0), gl.vec3f(0, 1, 0)
  #      )
  #      painter.state.perspective(
  #          35, 200 / 200, 0.005, 30.0
  #      )
  #      painter.state.rotate(angle.deg(self.deg), gl.vec3f(0, 1, 0))
  #      self.deg += 1
  #      light = self.renderer.new_light(
  #          gl.PointLightInfo(
  #              gl.vec3f(-1, -1, 0),
  #              gl.Color3f("red"),
  #              gl.Color3f("blue"),
  #              gl.Color3f("green"),
  #          )
  #      )
  #      with painter.bind([light, material.bindable(self.renderer)]):
  #          painter.draw([cube_view])


