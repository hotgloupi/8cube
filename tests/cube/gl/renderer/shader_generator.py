# -*- encoding: utf-8 -*-

import cube

w = cube.system.create_window(
    "Test shader gen", 320, 240, cube.gl.renderer.Name.OpenGL
)

gen = w.renderer.generate_shader(
    cube.gl.renderer.ShaderType.vertex
).input(
    cube.gl.renderer.ShaderParameterType.vec3, "cube_Position"
).input(
    cube.gl.renderer.ShaderParameterType.vec3, "cube_TexCoord"
).output(
    cube.gl.renderer.ShaderParameterType.vec4, "cube_FragColor"
)

class DiffuseRoutine(cube.gl.renderer.ShaderRoutine):

    def __init__(self):pass

    def source(self, proxy, name):
        super(DiffuseRoutine, self).source(proxy, name)

d = DiffuseRoutine()
d.source(gen, "oif")

print(gen.source())
print(gen.shader())
