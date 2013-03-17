# -*- encoding: utf-8 -*-


from cube.gl.renderer import ContentKind as Kind, DrawMode as Mode
from cube.gl import Mesh, vec3f
from cube.gl import Color3f as col3f


import cube
window = cube.system.create_window( "Test mesh",640, 480, cube.gl.renderer.Name.OpenGL)

w = h = 16

m = Mesh()
m.mode = Mode.quads
m.append(
    Kind.vertex,
    vec3f(0, 0, 0),
    vec3f(w, 0, 0),
    vec3f(w, 0, h),
    vec3f(0, 0, h),

    Kind.color,
    col3f('orange'),
    col3f('green'),
    col3f('gray'),
    col3f('blue'),
)
print(m)
view = m.view(window.renderer)
