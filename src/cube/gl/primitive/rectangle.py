# -*- encoding: utf-8 -*-

DEFAULT_FRAGMENT_SHADER = """
void main(void)
{
    gl_FragColor = gl_Color;
}
"""


DEFAULT_VERTEX_SHADER = """
uniform mat4 cube_ModelViewProjectionMatrix;
void main(void)
{
   gl_FrontColor = gl_Color;
   gl_Position = cube_ModelViewProjectionMatrix * gl_Vertex;
}
"""

class Rectangle2f:

    def __init__(self, renderer, position=(0,0), size=None, colors=None,
                 color='black', vertex_shader=None, fragment_shader=None):
        if colors is None:
            colors = [color]

        self.__colors = list(
            cube.gl.Color4f(color) for color in colors
        )
        assert self.__colors
        if len(self.__colors < 4):
            self.__colors.extend(
                [self.__colors[-1]] * (4 - len(self.__colors))
            )
        self.__position = cube.gl.Vector2f(position)
        if size is None:
            raise Exception("You have to provide a size")
        if vertex_shader is None:
            vertex_shader = DEFAULT_VERTEX_SHADER
        if fragment_shader is None:
            fragment_shader = DEFAULT_FRAGMENT_SHADER

        self.__size = cube.gl.Vector2f(size)

        self.__vb = renderer.new_vertex_buffer()
        x, y, w, h = (
            self.__position.x, self.__position.y
            self.__size.x, self.__size.y
        )
        self.__vb.push_static_content(
            gl.ContentKind.vertex,
            list(gl.Vector2f(*v) for v in [
                (x, y),
                (x + w, y),
                (x + w, y + h),
                (x, y + h)
            ])
        )
        self.__vb.push_static_content(
            gl.ContentKind.color,
            [
                gl.Color3f('orange'),
                gl.Color3f('green'),
                gl.Color3f('gray'),
                gl.Color3f('blue'),
            ]
        )
        self.__vb.finalize()
        self.__indices = renderer.new_index_buffer()
        self.__indices.push_static_content(
            gl.ContentKind.index,
            [ 0, 1, 2, 3]
        )
        self.__indices.finalize()

        self.__sp  = renderer.new_shader_program()
        fs = renderer.new_fragment_shader()
        fs.push_source(fragment_shader)
        self.__sp.push_shader(fs)
        del fs
        vs = renderer.new_vertex_shader()
        vs.push_source(vertex_shader)
        self.__sp.push_shader(vs)
        del vs
        self.__sp.finalize()
        self.__sp.parameter("cube_ModelViewProjectionMatrix")

    def draw(self, painter):
        painter.bind(self.__sp)
        painter.bind(self.__vb)
        painter.draw(self.__indices)
