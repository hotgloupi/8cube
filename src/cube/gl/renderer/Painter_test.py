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

from unittest import TestCase

class PainterSetup:

    def setUp(self):
        self.window = create_window("Hello", 640, 480, WindowFlags.none, Name.OpenGL)
        self.renderer = self.window.renderer
        # XXX Should be created before any opengl objects, otherwise we got
        # an abort in pthred_unlock ...
        self.target = self.renderer.context.new_render_target()


        self.fs = self.renderer.new_fragment_shader(["""
            void main(void)
            {
                gl_FragColor = gl_Color;
            }
        """])
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
        x, y, w, h = (
            11, 42,
            220, 240
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
                    Color3f('#ccc'),
                    Color3f('#ccc'),
                    Color3f('#ccc'),
                    Color3f('#ccd'),
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

class _(PainterSetup, TestCase):

    def test_simple(self):
        from cube.resource import Resource
        import time
        print(self.shader, isinstance(self.shader, Resource))
        self.assertTrue(self.shader.managed)
        self.window.poll()
        while True:
            self.renderer.clear(1)
            with self.renderer.begin(mode_2d) as painter:
                painter.state.model = matrix.translate(painter.state.model, 10, 12, .0)
                with painter.bind([self.shader, self.vb]):
                    self.shader['cube_MVP'] = painter.state.mvp
                    painter.draw_elements(DrawMode.quads, self.indices, 0, 4)
            self.window.poll()
            self.window.swap_buffers()
            time.sleep(.1)
        time.sleep(1)

