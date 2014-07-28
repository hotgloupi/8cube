
from cube.test import Case
from cube.units import angle
from .camera import Camera
from cube import gl

class _(Case):

    def test_init(self):
        c = Camera()
        self.assertEqual(c.pitch, angle.rad(0))
        self.assertEqual(c.yaw, angle.rad(0))
        self.assertEqual(c.roll, angle.rad(0))
        self.assertEqual(c.position, gl.vec3f())
        self.assertFalse(c.has_frustum)

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
