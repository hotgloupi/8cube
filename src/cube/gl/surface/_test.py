import os
import pathlib
import sys

from cube import gl
from cube.test import Case

from . import Surface

DIR = pathlib.Path(os.path.dirname(os.path.abspath(__file__)))

class _(Case):

    def test_init(self):
        s = Surface(gl.PixelFormat.rgb, 10, 10)
        s = Surface(10, 10)
        s = Surface(width = 10, height = 10, format = gl.PixelFormat.rgba)

    def _compare_surface(self, s, name, delta = 0):
        path = DIR / (name + '.bmp')
        tmp = DIR / (name + '-tmp.bmp')
        s.save_bmp(tmp)
        if not path.exists():
            print("#" * 80)
            print("New surface test", name, "please copy")
            print("\t", tmp)
            print('to')
            print("\t", path)
            print("#" * 80)
            sys.exit(1)
        if Surface(path).difference(Surface(tmp)) > delta:
            raise Exception(
                "The reference surface %s differs from %s" % (path, tmp)
            )
        tmp.unlink()

    def test_fill_rect_rgb(self):
        s = Surface(100, 100)
        s.fill_rect(gl.recti(0, 0, 10, 10), gl.col3f("red"));
        s.fill_rect(gl.recti(0, 90, 10, 100), gl.col3f("green"));
        s.fill_rect(gl.recti(90, 0, 100, 10), gl.col3f("blue"));
        s.fill_rect(gl.recti(90, 90, 100, 100), gl.col3f("white"));
        self._compare_surface(s, 'test_fill_rect_rgb')
