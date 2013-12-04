from cube import scene
from cube import gl
from cube.units import angle

from unittest import TestCase
from cube.gl.renderer.Painter_test import PainterSetup, painter_test

class _(PainterSetup, TestCase):

    @painter_test(gl.mode_2d)
    def test_sphere(self, painter):
        s = scene.from_string("s 0 0 0 1", "nff")
        self.assertIsNotNone(s)
        self.assertEqual(len(s.meshes), 1)
        self.assertEqual(len(s.materials), 1)
        mesh = s.meshes[0]
        mat = s.materials[0]
        self.assertIsNotNone(mesh)
        self.assertIsNotNone(mat)
        mat.ambient = gl.Color3f("white")

        painter.state.look_at(
            gl.vec3f(0, .8, -3), gl.vec3f(0, 0, 0), gl.vec3f(0, 1, 0)
        )
        painter.state.perspective(
            45, 200 / 200, 0.005, 30.0
        )
        with painter.bind([mat.bindable(self.renderer)]):
            painter.draw([mesh.view(self.renderer)])

    @painter_test(gl.mode_3d)
    def test_sphere_light(self, painter):
        s = scene.from_string("s 0 0 0 1", "nff")
        self.assertIsNotNone(s)
        self.assertEqual(len(s.meshes), 1)
        self.assertEqual(len(s.materials), 1)
        mesh = s.meshes[0]
        mat = s.materials[0]
        self.assertIsNotNone(mesh)
        self.assertIsNotNone(mat)
        mat.ambient = gl.Color3f("#222")
        mat.shading_model = gl.material.ShadingModel.gouraud
        mat.diffuse = gl.Color3f("white")
        mat.specular = gl.Color3f("white")
        mat.shininess = 3

        painter.state.look_at(
            gl.vec3f(0, .8, -3), gl.vec3f(0, 0, 0), gl.vec3f(0, 1, 0)
        )
        painter.state.perspective(
            45, 200 / 200, 0.005, 30.0
        )
        light = self.renderer.new_light(
            gl.PointLightInfo(
                gl.vec3f(5, -1, 0),
                gl.Color3f("black"),
                gl.Color3f("#0f0f0f"),
            )
        )
        with painter.bind([light, mat.bindable(self.renderer)]):
            painter.draw([mesh.view(self.renderer)])
