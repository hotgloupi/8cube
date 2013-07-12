# -*- encoding: utf-8 -*-

import cube

w = cube.system.create_window(
    "Test shader gen", 320, 240, cube.gl.renderer.Name.OpenGL
)

gen = w.renderer.generate_shader(
    cube.gl.renderer.ShaderType.fragment
).parameter(
    cube.gl.renderer.ShaderParameterType.vec4,
    "cube_AmbiantColor"
).parameter(
    cube.gl.renderer.ShaderParameterType.vec4,
    "cube_DiffuseColor"
).parameter(
    cube.gl.renderer.ShaderParameterType.vec4,
    "cube_SpecularColor"
).input(
    cube.gl.renderer.ShaderParameterType.vec3,
    "cube_VertexPosition",
    cube.gl.renderer.ContentKind.vertex,
).input(
    cube.gl.renderer.ShaderParameterType.vec3,
    "cube_TexCoord",
    cube.gl.renderer.ContentKind.tex_coord0
).output(
    cube.gl.renderer.ShaderParameterType.vec4,
    "cube_VertexColor"
)

class AmbiantRoutine(cube.gl.renderer.ShaderRoutine):

    def __init__(self, color=cube.gl.Color3f("pink")):
        super(AmbiantRoutine, self).__init__()

    def source(self, proxy, name):
        return """void %(routine_name)s()
{
    cube_FragColor = %(var_name)s
}
        """ % {
            'routine_name': name,
            'var_name': 'cube_AmbiantColor',
        }

    def is_applicable(self, shader_type):
        return shader_type in [
            cube.gl.renderer.ShaderType.vertex,
        ]



#d = AmbiantRoutine()
#print("source =", d.source(gen, "oif"))
#print("is applicable =", d.is_applicable(cube.gl.renderer.ShaderType.vertex))

print(gen.source())
#print(gen.shader())
