# -*- encoding: utf8 -*-

from cube.test import Case

from .angle import Angle, rad, deg, rad_value, deg_value

class _(Case):

    def test_null(self):
        a = Angle()
        self.assertEqual(rad_value(a), 0)
        self.assertEqual(deg_value(a), 0)

    def test_sum(self):
        self.assertAlmostEqual((deg(180) + rad(-3.14)).degree, 0, 1)
        self.assertAlmostEqual((deg(180) - rad(3.14)).degree, 0, 1)

    def test_value(self):
        self.assertEqual(deg(180).degree, 180)
        self.assertEqual(deg(-190).degree, -190)
        self.assertEqual(rad(12).radian, 12)

    def test_convert(self):
        self.assertAlmostEqual(deg(180).radian, rad(3.14159265).radian, 1e-5)

    def test_equals(self):
        self.assertEqual(deg(0), deg(0))
        self.assertEqual(deg(0), rad(0))

    def test_scale(self):
        a = deg(1)
        self.assertEqual(a * 2, deg(2))
        self.assertEqual(a * 2.0, deg(2))

