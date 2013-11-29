
from cube import gl
from cube.units import angle

from unittest import TestCase

class _(TestCase):

    def test_init(self):
        s = gl.State(gl.mode_2d)

    def test_no_copy(self):
        s = gl.State(gl.mode_2d)
        self.assertIs(s, s.scale(1,2,3))
        self.assertIs(s, s.translate(1,2,3))
        self.assertIs(s, s.rotate(angle.deg(0), gl.vec3f(1,2,3)))
        self.assertIs(s, s.look_at(
            gl.vec3f(0,0,0), gl.vec3f(1,2,3), gl.vec3f(0, 1, 0)
        ))
        self.assertIs(s, s.perspective(45, 1, 32, 32))
        self.assertIs(s, s.ortho(0, 0, 200, 200))

        self.assertIs(
            s,
            s.scale(1,2,3).translate(1,2,3).rotate(
                angle.deg(0), gl.vec3f(1,2,3)
            ).look_at(
                gl.vec3f(0,0,0), gl.vec3f(1,2,3), gl.vec3f(0, 1, 0)
            ).perspective(45, 1, 32, 32).ortho(0, 0, 200, 200)
        )

    def test_look_at(self):
        s = gl.State(gl.mode_2d)
        m = gl.Matrix44f(s.model)
        v = gl.Matrix44f(s.view)
        p = gl.Matrix44f(s.projection)
        mvp = gl.Matrix44f(s.mvp)

        self.assertEqual(m, s.model)
        self.assertEqual(v, s.view)
        self.assertEqual(p, s.projection)
        self.assertEqual(mvp, s.mvp)

        s.look_at(gl.vec3f(0,0,0), gl.vec3f(1,2,3), gl.vec3f(0, 1, 0))

        self.assertEqual(m, s.model)
        self.assertNotEqual(v, s.view)
        self.assertEqual(p, s.projection)
        self.assertNotEqual(mvp, s.mvp)


    def test_perspective(self):
        s = gl.State(gl.mode_2d)
        m = gl.Matrix44f(s.model)
        v = gl.Matrix44f(s.view)
        p = gl.Matrix44f(s.projection)
        mvp = gl.Matrix44f(s.mvp)

        self.assertEqual(m, s.model)
        self.assertEqual(v, s.view)
        self.assertEqual(p, s.projection)
        self.assertEqual(mvp, s.mvp)

        s.perspective(45, 1, 0.1, 300)

        self.assertEqual(m, s.model)
        self.assertEqual(v, s.view)
        self.assertNotEqual(p, s.projection)
        self.assertNotEqual(mvp, s.mvp)
