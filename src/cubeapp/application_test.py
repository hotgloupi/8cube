from .application import Application

from cube.test import Case

class _(Case):

    def test_init(self):
        app = Application()
