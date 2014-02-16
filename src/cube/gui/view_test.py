from .view import View

from cube.test import Case

class _(Case):

    def test_refcount(self):
        v = View(tag = 'test')
        import gc
        self.assertEqual(len(gc.get_referrers(v)), 1)
