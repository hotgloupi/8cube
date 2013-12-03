from cube.gl import \
        mode_2d, \
        make_vba, \
        ContentHint, \
        ContentKind, \
        Color3f, \
        vec2f, \
        DrawMode, \
        Name, \
        matrix, \
        Surface, \
        ShaderType
from cube import gl

from cube.system import create_window, WindowFlags
import os
from os import path
import time
import sys
import inspect

from unittest import TestCase

class PainterSetup:
    class VSRoutine(gl.ShaderRoutine):
        def source(self, lang):
            return """
               gl_Position = cube_MVP * vec4(cube_Vertex, 1);
               cube_Color = vec4(cube_VertexColor, 1);
            """
    vs_inputs = [
        (
            gl.ShaderParameterType.vec3,
            "cube_Vertex",
            gl.ContentKind.vertex
        ),
        (
            gl.ShaderParameterType.vec3,
            "cube_VertexColor",
            gl.ContentKind.color
        ),
    ]

    vs_outputs = [
        (
            gl.ShaderParameterType.vec4,
            "cube_Color",
            gl.ContentKind.color
        ),
    ]

    vs_parameters = [
        (
            gl.ShaderParameterType.mat4,
            "cube_MVP"
        ),
    ]

    class FSRoutine(gl.ShaderRoutine):
        def source(self, lang):
            return """
               cube_FragColor = cube_Color;
                """
    fs_inputs = [
        (
            gl.ShaderParameterType.vec4,
            "cube_Color",
            gl.ContentKind.color
        ),
    ]

    fs_outputs = [
        (
            gl.ShaderParameterType.vec4,
            "cube_FragColor",
            gl.ContentKind.color
        ),
    ]

    fs_parameters = []

    def setUp(self):
        self.window = create_window(
            "PainterSetup", 200, 200, WindowFlags.hidden, Name.OpenGL
        )
        self.renderer = self.window.renderer
        # Should be created before any shader
        self.target = self.renderer.context.new_render_target()

        gen = self.renderer.generate_shader(gl.ShaderType.vertex)
        for i in self.vs_inputs: gen.input(*i)
        for o in self.vs_outputs: gen.output(*o)
        for p in self.vs_parameters: gen.parameter(*p)
        self.vs = gen.routine(self.VSRoutine, "main").shader()

        gen = self.renderer.generate_shader(gl.ShaderType.fragment)
        for i in self.fs_inputs: gen.input(*i)
        for o in self.fs_outputs: gen.output(*o)
        for p in self.fs_parameters: gen.parameter(*p)
        self.fs = gen.routine(self.FSRoutine, "main").shader()

        x, y, w, h = (
            11, 42,
            120, 140
        )
        self.shader = self.renderer.new_shader_program([self.fs, self.vs])
        self.vb = self.renderer.new_vertex_buffer([
            gl.make_vba(
                gl.ContentKind.vertex,
                list(gl.vec3f(*v) for v in [
                    (x, y, 0),
                    (x + w, y, 0),
                    (x + w, y + h, 0),
                    (x, y + h, 0)
                ]),
                gl.ContentHint.static_content
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


def painter_test(mode, delta = 0.01):
    def _painter_test(func):
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

            with self.renderer.begin(mode) as painter:
                with painter.bind([self.target]):
                    self.renderer.clear(
                        gl.BufferBit.color | gl.BufferBit.depth
                    )
                    func(self, painter)
                self.target.save(img)

            if first_time:
                print("\n\n###############################################")
                print("##### NEW PAINTER TEST NEEDS MANUAL VALIDATION:\n")
                print("Check the output and copy the temporary file: \n\t>> %s" % img)
                print("to: \n\t>> %s" % thruth)
                print("\n###############################################\n")
                self.window.show()
                running = [1]
                self.renderer.clear(1)
                slot = self.window.inputs.on_quit.connect(lambda: running.pop())
                self.window.title = fname
                while running:
                    self.window.swap_buffers()
                    with self.renderer.begin(mode) as painter:
                        self.renderer.clear(
                            gl.BufferBit.color | gl.BufferBit.depth
                        )
                        func(self, painter)
                    self.window.poll()
                    self.renderer.flush()
                slot.disconnect()
                print("\n\n###############################################")
                print("##### NEW PAINTER TEST NEEDS MANUAL VALIDATION:\n")
                print("Check the output and copy the temporary file: \n\t>> %s" % img)
                print("to: \n\t>> %s" % thruth)
                print("\n###############################################\n")
                sys.exit(0)

            diff = Surface(img).difference(Surface(thruth))
            self.assertAlmostEqual(diff, 0, delta = delta,
                                   msg = "%s and %s are not the same" % (thruth, img))
            os.unlink(img)

        wrapper.__name__ = func.__name__
        return wrapper
    return _painter_test

class _(PainterSetup, TestCase):

    @painter_test(mode_2d, delta = 0.06)
    def test_simple(self, painter):
        with painter.bind([self.shader, self.vb]):
            self.shader['cube_MVP'] = painter.state.mvp
            painter.draw_elements(DrawMode.quads, self.indices, 0, 4)


    @painter_test(mode_2d, delta = 0.02)
    def test_transform(self, painter):
        with painter.bind([self.shader, self.vb]):
            painter.state.model = matrix.scale(
                painter.state.model,
                gl.vec3f(0.5, 0.5, 0.5)
            )
            painter.state.model = matrix.translate(
                painter.state.model,
                gl.vec3f(100, 75, 0)
            )
            self.shader['cube_MVP'] = painter.state.mvp
            painter.draw_elements(DrawMode.quads, self.indices, 0, 4)
