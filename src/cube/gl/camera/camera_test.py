
from cube.test import Case
from cube.units import angle
from . import Camera
from cube import gl

class _(Case):

    def test_init(self):
        c = Camera()
        self.assertEqual(c.pitch, angle.rad(0))
        self.assertEqual(c.yaw, angle.rad(0))
        self.assertEqual(c.roll, angle.rad(0))
        self.assertEqual(c.position, gl.vec3f())
        self.assertFalse(c.has_frustum)
        self.assertEqual(c.right, gl.vec3f(1, 0, 0))
        self.assertEqual(c.front, gl.vec3f(0, 0, -1))
        self.assertEqual(c.up, gl.vec3f(0, 1, 0))

    def test_pitch(self):
        c = Camera()
        c.pitch = angle.deg(90)
        self.assertEqual(c.pitch.degree, 90)
        c.pitch = angle.deg(-90)
        self.assertEqual(c.pitch.degree, 0)
        c.pitch = angle.deg(12)
        self.assertAlmostEqual(c.pitch.degree, 12, 1e-5)

    def test_roll(self):
        c = Camera()
        c.roll = angle.deg(90)
        self.assertAlmostEqual(c.roll.degree, 90, 1e-5)
        c.roll = angle.deg(-90)
        self.assertEqual(c.roll.degree, 0)
        c.roll = angle.deg(12)
        self.assertAlmostEqual(c.roll.degree, 12, 1e-5)

    def test_yaw(self):
        c = Camera()
        c.yaw = angle.deg(90)
        self.assertAlmostEqual(c.yaw.degree, 90, .2)
        c.yaw = angle.deg(-90)
        self.assertEqual(c.yaw.degree, 0)
        c.yaw = angle.deg(12)
        self.assertAlmostEqual(c.yaw.degree, 12, .2)


    def test_frustum(self):
        c = Camera()
        c.init_frustum(angle.deg(30), 1, 10, 20)
        self.assertTrue(c.has_frustum)
        f = c.frustum

    def test_look_at_identity(self):
        c = Camera()
        c.look_at(c.front)
        self.assertEqual(c.yaw, angle.deg(0))
        self.assertEqual(c.roll, angle.rad(0))
        self.assertEqual(c.pitch, angle.rad(0))

    def test_look_at_back(self):
        c = Camera()
        front, up, right = c.front, c.up, c.right
        c.look_at(-c.front)
        self.assertEqual(c.front, -front)
        self.assertEqual(c.up, -up)
        self.assertEqual(c.right, right)

        c.look_at(-c.front)
        self.assertEqual(c.front, front)
        self.assertEqual(c.up, up)
        self.assertEqual(c.right, right)

    def test_look_at_self(self):
        c = Camera()
        c.look_at(c.position)

    def test_look_at_normalized(self):
        c = Camera()
        front = c.front
        c.look_at(front * 2)
        self.assertEqual(c.front, front)

