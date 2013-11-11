#-*- encoding: utf8 -*-

from unittest import TestCase
from . import window
from cube.gl.renderer import Name

class Creation(TestCase):

    def setUp(self):
        self.context = window.create_renderer_context(
            640,
            480,
            window.WindowFlags.hidden,
            Name.OpenGL
        )
    def tearDown(self):
        window.delete_renderer_context(self.context)
        print("CONTEXT", self.context)

    def test_renderer_context(self):
        self.assertEqual(self.context.width, 640)
        self.assertEqual(self.context.height, 480)
        self.assertEqual(self.context.flags, window.WindowFlags.hidden)
        self.assertEqual(self.context.name, Name.OpenGL)
        self.context.size(12, 32)
        self.assertEqual(self.context.width, 12)
        self.assertEqual(self.context.height, 32)

    def test_create_window_from_context(self):
        w = window.create_window("LOL", self.context)
        self.assertEqual(w.title, "LOL")
        self.assertEqual(w.width, 640)
        self.assertEqual(w.height, 480)
        self.assertEqual(w.renderer_context.width, 640)
        self.assertEqual(w.renderer_context.height, 480)
        self.assertEqual(w.renderer_context.flags, window.WindowFlags.hidden)

        for attr in ['width', 'height', 'name', 'flags']:
            try:
                getattr(self.context, attr)
            except:
                pass
            else:
                self.fail("The context is stolen by the window, accessing it "
                          "should throw")
        window.delete_window(w)

    def test_create_window(self):
        w = window.create_window(
            "title",
            640,
            480,
            window.WindowFlags.hidden,
            Name.OpenGL
        )
        self.assertEqual(w.title, "title")
        self.assertEqual(w.width, 640)
        self.assertEqual(w.height, 480)
        self.assertEqual(w.renderer_context.width, 640)
        self.assertEqual(w.renderer_context.height, 480)
        window.delete_window(w)

    def test_create_multiple_window(self):
        w1 = window.create_window(
            "title1",
            640,
            480,
            window.WindowFlags.hidden,
            Name.OpenGL
        )
        self.assertEqual(w1.title, "title1")
        self.assertEqual(w1.width, 640)
        self.assertEqual(w1.height, 480)
        w2 = window.create_window(
            "title2",
            640,
            480,
            window.WindowFlags.hidden,
            Name.OpenGL
        )
        self.assertEqual(w2.title, "title2")
        self.assertEqual(w2.width, 640)
        self.assertEqual(w2.height, 480)
        window.delete_window(w1)
        window.delete_window(w2)

class _(TestCase):
    def setUp(self):
        self.window = window.create_window(
            "Testing window",
            640,
            480,
            window.WindowFlags.none,
            Name.OpenGL
        )
        self.window.poll()

    def tearDown(self):
        window.delete_window(self.window)

    def test_show_hide(self):
        self.window.show()
        self.window.poll()
        self.window.hide()
        self.window.poll()

    def test_minimize(self):
        self.window.show()
        self.window.minimize()
        self.window.poll()
        self.window.maximize()
        self.window.poll()

