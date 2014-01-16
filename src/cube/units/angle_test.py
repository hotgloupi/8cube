# -*- encoding: utf8 -*-

from cube.test import Case

from .angle import Angle, rad, deg, rad_value, deg_value

class _(Case):

    def test_angle(self):
        a = Angle()
        self.assertEqual(rad_value(a), 0)
        self.assertEqual(deg_value(a), 0)
