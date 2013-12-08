
from cube import gl
from cube.gl.renderer.State import StateHolder
from cube.units import angle

from unittest import TestCase

class _(TestCase):

    def setUp(self):
        self.s = StateHolder(gl.mode_2d).state

    def test_no_copy(self):
        self.assertIs(self.s, self.s.scale(1,2,3))
        self.assertIs(self.s, self.s.translate(1,2,3))
        self.assertIs(self.s, self.s.rotate(angle.deg(0), gl.vec3f(1,2,3)))
        self.assertIs(self.s, self.s.look_at(
            gl.vec3f(0,0,0), gl.vec3f(1,2,3), gl.vec3f(0, 1, 0)
        ))
        self.assertIs(self.s, self.s.perspective(45, 1, 31, 32))
        self.assertIs(self.s, self.s.ortho(0, 0, 200, 200))

        self.assertIs(
            self.s,
            self.s.scale(1,2,3).translate(1,2,3).rotate(
                angle.deg(0), gl.vec3f(1,2,3)
            ).look_at(
                gl.vec3f(0,0,0), gl.vec3f(1,2,3), gl.vec3f(0, 1, 0)
            ).perspective(45, 1, 31, 32).ortho(0, 0, 200, 200)
        )

    def test_look_at(self):
        m = gl.mat4f(self.s.model)
        v = gl.mat4f(self.s.view)
        p = gl.mat4f(self.s.projection)
        mvp = gl.mat4f(self.s.mvp)

        self.assertEqual(m, self.s.model)
        self.assertEqual(v, self.s.view)
        self.assertEqual(p, self.s.projection)
        self.assertEqual(mvp, self.s.mvp)

        self.s.look_at(gl.vec3f(0,0,0), gl.vec3f(1,2,3), gl.vec3f(0, 1, 0))

        self.assertEqual(m, self.s.model)
        self.assertNotEqual(v, self.s.view)
        self.assertEqual(p, self.s.projection)
        self.assertNotEqual(mvp, self.s.mvp)


    def test_perspective(self):
        m = gl.mat4f(self.s.model)
        v = gl.mat4f(self.s.view)
        p = gl.mat4f(self.s.projection)
        mvp = gl.mat4f(self.s.mvp)

        self.assertEqual(m, self.s.model)
        self.assertEqual(v, self.s.view)
        self.assertEqual(p, self.s.projection)
        self.assertEqual(mvp, self.s.mvp)

        self.s.perspective(45, 1, 0.1, 300)

        self.assertEqual(m, self.s.model)
        self.assertEqual(v, self.s.view)
        self.assertNotEqual(p, self.s.projection)
        self.assertNotEqual(mvp, self.s.mvp)

    def test_ortho(self):
        m = gl.mat4f(self.s.model)
        v = gl.mat4f(self.s.view)
        p = gl.mat4f(self.s.projection)
        mvp = gl.mat4f(self.s.mvp)

        self.assertEqual(m, self.s.model)
        self.assertEqual(v, self.s.view)
        self.assertEqual(p, self.s.projection)
        self.assertEqual(mvp, self.s.mvp)

        self.s.ortho(0, 0, 200, 200)

        self.assertEqual(m, self.s.model)
        self.assertEqual(v, self.s.view)
        self.assertNotEqual(p, self.s.projection)
        self.assertNotEqual(mvp, self.s.mvp)
