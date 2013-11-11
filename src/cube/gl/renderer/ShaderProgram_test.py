# -*- encoding: utf8 -*-

from unittest import TestCase

from .Shader_test import ShaderSetup

class ShaderProgramSetup(ShaderSetup):

    def setUp(self):
        super().setUp()
        self.shader = self.renderer.new_shader_program([self.fs, self.vs])

class ShaderProgramTestCase(ShaderProgramSetup, TestCase):

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
