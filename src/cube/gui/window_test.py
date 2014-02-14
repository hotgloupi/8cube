
from .window import Window

from cube.test import Case

class _(Case):

    def test_ctor(self):
        w = Window("test", 640, 480)
        self.assertEqual(w.root_widget.name, "root widget")
