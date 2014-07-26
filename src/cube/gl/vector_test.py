from cube.test import Case
from . import vec3f

class _(Case):

    def test_equals(self):
        self.assertEqual(vec3f(), vec3f())
        self.assertEqual(vec3f(0), vec3f(0))
        self.assertEqual(vec3f(0,0,0), vec3f(0,0,0))
