"""Graphic Library

cube graphic library consist on a set of objects which abstract the rendering
process.

Exported modules:
    matrix
    font

Exported classes:

    Camera
    Color{3,4}f
    Cube3{f,d,i,u,il,ul}
    Font
    Matrix44f
    Text
    Vector{2,3}{f,d,i,u,il,ul}
    Viewport
    Drawable, Painter, Renderer, State, RendererType, Shader, VertexBuffer,
    ShaderProgram, ShaderProgramParameter


Exported enumerations:

    RendererMode (_2d, _3d)
    ContentKind (vertex, index, color, normal, tex_coord0, tex_coord1,
                 tex_coord2)
    DrawMode (points, lines, line_strip, line_loop, triangles, triangle_strip,
              triangle_fan, quads, quad_strip, polygon)
    BufferBit (color, depth, stencil)
    XAxis (left, right)
    YAxis (top, down)
    ZAxis (front, back)

"""

from . import font
from . import matrix

from .camera import Camera
from .color import Color3f, Color4f
from .cube import Cube3f, Cube3d, Cube3i, Cube3u, Cube3il, Cube3ul
from .renderer import *
from .text import Text
from .font import Font
from .matrix import Matrix44f
from .vector import Vector2f, Vector2d, Vector2i, Vector2u, Vector2il, Vector2ul
from .vector import Vector3f, Vector3d, Vector3i, Vector3u, Vector3il, Vector3ul
from .viewport import Viewport

#from .test import test_all
