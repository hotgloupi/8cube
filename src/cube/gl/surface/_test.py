
from . import Surface
from cube import gl

from cube.test import Case

class _(Case):

    def test_init(self):
        s = Surface(gl.PixelFormat.rgb, 10, 10, gl.PixelFormat.rgb,
                    gl.ContentPacking.uint8, None)
