# -*- encoding: utf8 -*-

from .plane import Planef, Planed
from . import vec3f

from cube.test import Case

class _(Case):

    def test_normalized(self):
        p = Planef(vec3f(0,0,0), vec3f(0,1,0))

        #print(p, p.normal, p.coef)
        p = Planef(vec3f(1,0,0),vec3f(0,0,0), vec3f(0,0,1))
        #print(p, p.normal, p.coef)

    def test_distance(self):
        p = Planef(vec3f(0,0,0), vec3f(0,1,0))
        self.assertEqual(p.distance(vec3f(0,10,0)), 10)
