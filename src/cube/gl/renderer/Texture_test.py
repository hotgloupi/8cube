from .Painter_test import PainterSetup, painter_test

from cube import gl
from cube.test import Case

class _(PainterSetup, Case):
    class VSRoutine(gl.ShaderRoutine):
        def source(self, lang):
            return """
                gl_Position = cube_MVP * vec4(cube_Vertex, 1);
                cube_TexCoord0 = cube_VertexTexCoord0;
            """

    vs_inputs = [
        (
            gl.ShaderParameterType.vec3,
            "cube_Vertex",
            gl.ContentKind.vertex
        ),
        (
            gl.ShaderParameterType.vec2,
            "cube_VertexTexCoord0",
            gl.ContentKind.tex_coord0
        ),
    ]

    vs_outputs = [
        (
            gl.ShaderParameterType.vec2,
            "cube_TexCoord0",
            gl.ContentKind.tex_coord0
        ),
    ]

    class FSRoutine(gl.ShaderRoutine):
        def source(self, lang):
            return """
                cube_FragColor = texture2D(sampler0, vec2(cube_TexCoord0));
            """

    fs_parameters = [
        (
            gl.ShaderParameterType.sampler2d,
            "sampler0"
        ),
    ]

    fs_inputs = [
        (
            gl.ShaderParameterType.vec2,
            "cube_TexCoord0",
            gl.ContentKind.tex_coord0
        ),
    ]

    fs_outputs = [
        (
            gl.ShaderParameterType.vec4,
            "cube_FragColor",
            gl.ContentKind.color
        ),
    ]

    def setUp(self):
        super().setUp()

        x, y, w, h = (
            0, 0,
            200, 200
        )
        self.vb = self.renderer.new_vertex_buffer([
            gl.make_vba(
                gl.ContentKind.vertex,
                [
                    gl.vec2f(x, y),
                    gl.vec2f(x, y + h),
                    gl.vec2f(x + w, y + h),
                    gl.vec2f(x + w, y),
                ],
                gl.ContentHint.static_content
            ),
            gl.make_vba(
                gl.ContentKind.tex_coord0,
                [
                    gl.vec2f(0, 0),
                    gl.vec2f(0, 1),
                    gl.vec2f(1, 1),
                    gl.vec2f(1, 0),
                ],
                gl.ContentHint.static_content
            )
        ])
        self.indices = self.renderer.new_index_buffer(
            gl.make_vba(
                gl.ContentKind.index,
                [ 0, 1, 2, 3],
                gl.ContentHint.static_content
            )
        )
        from os import path
        self.texture = self.renderer.new_texture(
            gl.Surface(path.join(path.dirname(__file__), "texture_test.bmp"))
        )


    @painter_test(gl.mode_2d)
    def test_simple_load(self, painter):
        with painter.bind([self.texture, self.shader, self.vb]):
            self.shader['cube_MVP'] = painter.state.mvp
            self.shader['sampler0'] = self.texture
            painter.draw_elements(gl.DrawMode.quads, self.indices, 0, 4)

    @painter_test(gl.mode_2d)
    def test_draw_arrays(self, painter):
        with painter.bind([self.texture, self.shader]):
            self.shader['cube_MVP'] = painter.state.mvp
            self.shader['sampler0'] = self.texture
            painter.draw_arrays(gl.DrawMode.quads, self.vb, 0, 4)

