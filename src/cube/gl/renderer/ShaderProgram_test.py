# -*- encoding: utf8 -*-

from cube.test import Case

from cube.gl.matrix import mat4f
from cube.gl import vec3f

from .Shader_test import ShaderSetup

class ShaderProgramSetup(ShaderSetup):

    def setUp(self):
        super().setUp()
        self.shader = self.renderer.new_shader_program([self.fs, self.vs])

    def tearDown(self):
        self.shader = None
        super().tearDown()

class ShaderProgramTestCase(ShaderProgramSetup, Case):

    def test_init(self):
        pass

    def test_init_invalid_args(self):
        test_args = [
            (),
            ([],),
            (None,),
            ([None],),
            ([32, "lol"],),
            ([], []),
            ([self.shader],),
            ([self.renderer],),
        ]
        for args in test_args:
            try:
                self.renderer.new_shader_program(*args)
            except:
                pass
            else:
                self.fail("Shouldn't be able to create a program from %s" % args)

    def test_parameter(self):
        self.vs = self.renderer.new_vertex_shader([
            """
            uniform mat4 mat44f;
            uniform vec3 vec3f;
            uniform float floats[4];
            uniform float floats2[4];
            void main(void)
            {
               gl_FrontColor = gl_Color;
               for (int i = 0; i < 4; i++)
                   gl_FrontColor.r *= floats[i] + floats2[i];
               gl_Position = mat44f * vec4(vec3f, 0);
            }
            """
        ])
        self.fs = self.renderer.new_fragment_shader(["""
            void main(void)
            {
                gl_FragColor = gl_Color;
            }
        """])

        shader = self.renderer.new_shader_program([self.vs, self.fs])
        shader["vec3f"] = vec3f()
        shader["mat44f"] = mat4f()
        shader["floats"] = 12.0
        shader["floats"][1] = 12.0
        shader["floats"][0] = 12.0


