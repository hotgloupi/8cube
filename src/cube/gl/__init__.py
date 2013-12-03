"""Graphic Library

cube graphic library consist on a set of objects which abstract the rendering
process.

Exported modules:
    matrix
    font
    frustum

Exported classes:

    Camera
    Color{3,4}f
    Cube3{f,d,i,u,il,ul}
    Font
    Light, SpotLightInfo, PointLightInfo, DirectionalLightInfo
    Material
    mat4f
    Mesh
    Plane{f,d}
    Sphere{f,d,i,u,il,ul}
    Text
    Vector{2,3,4}{f,d,i,u,il,ul} (and aliases vec*)
    Viewport
    Drawable, Painter, Renderer, State, RendererType, Shader, VertexBuffer,
    ShaderProgram, ShaderProgramParameter
    Surface


Exported enumerations:

    Name (OpenGL, DirectX)
    RendererMode (_2d, _3d)
    ContentKind (vertex, index, color, normal, tex_coord0, tex_coord1,
                 tex_coord2)
    DrawMode (points, lines, line_strip, line_loop, triangles, triangle_strip,
              triangle_fan, quads, quad_strip, polygon)
    BufferBit (color, depth, stencil)
    ShaderType (fragment, vertex)
    XAxis (left, right)
    YAxis (top, down)
    ZAxis (front, back)
    TextureMapping (uv, sphere, cylinder, box, plane)
    StackOperation (multiply, add, subtract, divide, smooth_add, signed_add)
    TextureMapMode (wrap, clamp, decal, mirror)
    BlendMode (basic, additive)
    TextureType (diffuse, specular, ambient, emissive, height, normals,
                 shininess, opacity, displacement, lightmap, reflection)

    LightKind (point, spot, directional)
"""

from . import font
from . import frustum
from . import matrix

from .camera import Camera
from .color import Color3f, Color4f
from .cube import Cube3f, Cube3d, Cube3i, Cube3u, Cube3il, Cube3ul
from .text import Text
from .font import Font
from .material import *
from .matrix import mat4f
from .mesh import Mesh
from .plane import Planef, Planed
from .sphere import Spheref, Sphered, Spherei, Sphereu, Sphereil, Sphereul
from .surface import Surface
from .vector import Vector2f, Vector2d, Vector2i, Vector2u, Vector2il, Vector2ul
from .vector import Vector3f, Vector3d, Vector3i, Vector3u, Vector3il, Vector3ul
from .vector import Vector4f, Vector4d, Vector4i, Vector4u, Vector4il, Vector4ul

vec2f = Vector2f
vec2d = Vector2d
vec2i = Vector2i
vec2u = Vector2u
vec2il = Vector2il
vec2ul = Vector2ul
vec3f = Vector3f
vec3d = Vector3d
vec3i = Vector3i
vec3u = Vector3u
vec3il = Vector3il
vec3ul = Vector3ul
vec4f = Vector4f
vec4d = Vector4d
vec4i = Vector4i
vec4u = Vector4u
vec4il = Vector4il
vec4ul = Vector4ul

from .viewport import Viewport

from .renderer import *
