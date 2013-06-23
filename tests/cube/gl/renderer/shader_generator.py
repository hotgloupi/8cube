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
).parameter(
    cube.gl.renderer.ShaderParameterType.vec4, "cube_DiffuseColor"
)

class AmbiantRoutine(cube.gl.renderer.ShaderRoutine):

    def __init__(self, color=cube.gl.Color3f("pink")):
        super(AmbiantRoutine, self).__init__()

    def source(self, proxy, name):
        return super(AmbiantRoutine, self).source(proxy, name)

d = AmbiantRoutine()
print("source =", d.source(gen, "oif"))
print("is applicable =", d.is_applicable())

print(gen.source())
print(gen.shader())
