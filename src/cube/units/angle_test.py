# -*- encoding: utf8 -*-

from unittest import TestCase

from .angle import Angle, rad, deg, rad_value, deg_value

class _(TestCase):

    def test_angle(self):
        a = Angle()
        self.assertEqual(rad_value(a), 0)
        self.assertEqual(deg_value(a), 0)
