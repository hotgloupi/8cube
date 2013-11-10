#-*- encoding: utf8 -*-

from unittest import TestCase
from . import window
from cube.gl.renderer import Name

class _(TestCase):

    def test_renderer_context(self):
        ctx = window.create_renderer_context(640, 480, window.WindowFlags.none, Name.OpenGL)
