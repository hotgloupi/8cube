from cube.gl import \
        mode_2d, \
        make_vba, \
        ContentHint, \
        ContentKind, \
        Color3f, \
        vec2f, \
        DrawMode, \
        Name, \
        matrix
from cube.system import create_window, WindowFlags
import os
from os import path
import time
import sys
import inspect

from unittest import TestCase

class PainterSetup:

    def setUp(self):
        self.window = create_window("PainterSetup", 200, 200, WindowFlags.hidden, Name.OpenGL)
        self.renderer = self.window.renderer
        # Should be created before any shader
        self.target = self.renderer.context.new_render_target()

        self.vs = self.renderer.new_vertex_shader([
            """
            uniform mat4 cube_MVP;
            void main(void)
            {
               gl_FrontColor = gl_Color;
               gl_Position = cube_MVP * gl_Vertex;
            }
            """
        ])

        self.fs = self.renderer.new_fragment_shader(["""
            void main(void)
            {
                gl_FragColor = gl_Color;
            }
        """])

        x, y, w, h = (
            11, 42,
            120, 140
        )
        self.shader = self.renderer.new_shader_program([self.fs, self.vs])
        self.vb = self.renderer.new_vertex_buffer([
            make_vba(
                ContentKind.vertex,
                list(vec2f(*v) for v in [
                    (x, y),
                    (x + w, y),
                    (x + w, y + h),
                    (x, y + h)
                ]),
                ContentHint.static_content
            ),
            make_vba(
                ContentKind.color,
                [
                    Color3f('#e00'),
                    Color3f('#00f'),
                    Color3f('#ffc'),
                    Color3f('#9cd'),
                ],
                ContentHint.static_content
            )
        ])
        self.indices = self.renderer.new_index_buffer(
            make_vba(
                ContentKind.index,
                [ 0, 1, 2, 3],
                ContentHint.static_content
            )
        )

def painter_test(func):
    def wrapper(self):
        fname = list(filter(
            lambda e: bool(e),
            self.__class__.__module__.lower().replace('_test', '').split('.')
        ))[-1]
        cls = self.__class__.__name__.lower().replace('_', '')
        if cls: fname += '_' + cls
        fname += '_' + func.__name__
        source_dir = path.dirname(inspect.getfile(self.__class__))
        thruth = path.join(source_dir, "%s.bmp" % fname)
        img = path.join(source_dir, "%s-tmp.bmp" % fname)
        first_time = not path.exists(thruth)

        if first_time:
            img = thruth

        with self.renderer.begin(mode_2d) as painter:
            with painter.bind([self.target]):
                self.renderer.clear(1)
                func(self, painter)
            self.target.save(img)

        if first_time:
            self.window.show()
            running = [1]
            self.renderer.clear(1)
            slot = self.window.inputs.on_quit.connect(lambda: running.pop())
            self.window.title = fname
            while running:
                self.window.swap_buffers()
                with self.renderer.begin(mode_2d) as painter:
                    self.renderer.clear(1)
                    func(self, painter)
                self.window.poll()
            slot.disconnect()
            print("Compare the previous output with the file %s" % img)
            sys.exit(0)

        with open(thruth, 'rb') as f1:
            with open(img, 'rb') as f2:
                self.assertTrue(f1.read() == f2.read(),
                                "%s and %s are not the same" % (thruth, img))
        os.unlink(img)

    wrapper.__name__ = func.__name__
    return wrapper

class _(PainterSetup, TestCase):

    @painter_test
    def test_simple(self, painter):
        with painter.bind([self.shader, self.vb]):
            self.shader['cube_MVP'] = painter.state.mvp
            painter.draw_elements(DrawMode.quads, self.indices, 0, 4)

