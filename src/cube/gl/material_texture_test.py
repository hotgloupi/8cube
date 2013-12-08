from .material import Material
from .renderer.Painter_test import PainterSetup, painter_test
from cube.units import angle
from cube import gl
from unittest import TestCase


class _(PainterSetup, TestCase):
    __cube = None

    @property
    def cube(self):
        if self.__cube is not None:
            return self.__cube
        self.__cube = gl.Mesh()

        vertices = [
            #front face
            (-.5, -.5,  .5),
            (-.5,  .5,  .5),
            (.5,  .5,  .5),
            (.5, -.5,  .5),

            #back face
            (-.5, -.5, -.5),
            (-.5,  .5, -.5),
            (.5,  .5, -.5),
            (.5, -.5, -.5),
        ]
        colors = [
            #front face is blue
            (0, 0, 1),
            (0, 0, 1),
            (0, 0, 1),
            (0, 0, 1),
            #back face is red
            (1, 0, 0),
            (1, 0, 0),
            (1, 0, 0),
            (1, 0, 0),
        ]
        tex_coords = [
            (0, 1),
            (0, 0),
            (1, 0),
            (1, 1),

            (1, 1),
            (1, 0),
            (0, 0),
            (0, 1),
        ]
        indices = [
            0,2,1,  0,3,2,
            4,3,0,  4,7,3,
            4,1,5,  4,0,1,
            3,6,2,  3,7,6,
            1,6,5,  1,2,6,
            7,5,6,  7,4,5
        ]
        self.__cube.mode = gl.DrawMode.triangles

        for i in indices:
            self.__cube.kind = gl.ContentKind.vertex
            self.__cube.append(gl.vec3f(*vertices[i]))
            self.__cube.kind = gl.ContentKind.color
            self.__cube.append(gl.Color3f(*colors[i]))
            self.__cube.kind = gl.ContentKind.tex_coord0
            self.__cube.append(gl.vec2f(*tex_coords[i]))


        return self.__cube

    deg = 0


    @painter_test(gl.mode_3d)
    def test_texture(self, painter):
        material = Material()
        material.add_color(
            gl.ShaderParameterType.vec3,
            gl.StackOperation.add
        )
        import os
        material.add_texture(
            os.path.join(os.path.dirname(__file__), "material_test.bmp"),
            gl.TextureType.ambient,
            gl.TextureMapping.uv,
            gl.StackOperation.add,
            gl.TextureMapMode.wrap,
            gl.BlendMode.basic
        )
        cube_view = self.cube.view(self.renderer)
        painter.state.look_at(
            gl.vec3f(0, .8, -3), gl.vec3f(0, 0, 0), gl.vec3f(0, 1, 0)
        )
        painter.state.perspective(
            35, 200 / 200, 0.005, 30.0
        )
        painter.state.rotate(angle.deg(42), gl.vec3f(1, 1, 0))
        self.deg += 1
        with painter.bind([material.bindable(self.renderer)]):
            painter.draw([cube_view])


