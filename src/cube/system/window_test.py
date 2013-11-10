#-*- encoding: utf8 -*-

from unittest import TestCase
from . import window
from cube.gl.renderer import Name

class _(TestCase):

    def setUp(self):
        self.context = window.create_renderer_context(
            640,
            480,
            window.WindowFlags.none,
            Name.OpenGL
        )

    def test_renderer_context(self):
        self.assertEqual(self.context.width, 640)
        self.assertEqual(self.context.height, 480)
        self.assertEqual(self.context.flags, window.WindowFlags.hidden)
        self.assertEqual(self.context.name, Name.OpenGL)


    def test_create_window_from_context(self):
        w = window.create_window("LOL", self.context)
