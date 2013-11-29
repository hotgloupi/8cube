# -*- encoding: utf8 -*-

from .mesh import Mesh
from cube.gl.renderer.Painter_test import painter_test, PainterSetup
from cube import gl

from unittest import TestCase

class _(PainterSetup, TestCase):

    def test_simple(self):
        m = Mesh()
        data = [
            (0, 0, 0),
            (1, 0, 0),
            (2, 0, 0),
            (0, 1, 0),
            (0, 2, 0),
            (0, 0, 1),
            (0, 0, 2),
        ]
        self.assertEqual(m.kind, gl.ContentKind.vertex)
        self.assertEqual(m.mode, gl.DrawMode.triangle_strip)
        for v in data:
            m.append(gl.vec3f(*v))
        self.assertEqual(str(m), '<Mesh 7 vertice>')
        m.kind = gl.ContentKind.color
        for v in data:
            m.append(gl.Color3f(*v))
        self.assertEqual(str(m), '<Mesh 7 vertice, 7 colors3>')
        m.kind = gl.ContentKind.normal
        for v in data:
            m.append(gl.vec3f(*v))
        self.assertEqual(str(m), '<Mesh 7 vertice, 7 normals, 7 colors3>')

    @painter_test(gl.mode_2d)
    def test_paint(self, painter):
        m = Mesh()
        m.mode = gl.DrawMode.quads
        m.append(gl.vec3f(0, 0, 0))
        m.append(gl.vec3f(0, 100, 0))
        m.append(gl.vec3f(100, 100, 0))
        m.append(gl.vec3f(100, 0, 0))
        m.kind = gl.ContentKind.color
        m.append(gl.Color3f("red"))
        m.append(gl.Color3f("blue"))
        m.append(gl.Color3f("green"))
        m.append(gl.Color3f("gray"))

        view = m.view(self.renderer)

        with painter.bind([self.shader]):
            self.shader['cube_MVP'] = painter.state.mvp
            painter.draw([view])
