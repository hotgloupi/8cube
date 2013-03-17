# -*- encoding: utf-8 -*-

from cube.gl.renderer import ContentKind as Kind, DrawMode as Mode
from cube.gl import Mesh, vec3f
from cube.gl import Color3f as col3f

vertex_shader = """
uniform mat4 cube_ModelViewProjectionMatrix;
void main()
{
    gl_FrontColor = gl_Color;
    gl_Position = cube_ModelViewProjectionMatrix * gl_Vertex;
}
"""

fragment_shader = """
void main()
{
    gl_FragColor = gl_Color;
}
"""

import cube
class App:
    def __init__(self):
        self.window = cube.system.create_window( "Test mesh",640, 480, cube.gl.renderer.Name.OpenGL)
        w = h = 1
        m = Mesh()
        m.mode = Mode.quads
        m.append(
            Kind.vertex,
            vec3f(0, 0, 0),
            vec3f(w, 0, 0),
            vec3f(w, 0, h),
            vec3f(0, 0, h),
            #vec3f(w, h, 0),
            #vec3f(-w, h, 0),
            #vec3f(-w, -h, 0),
            #vec3f(w, -h, 0),

            Kind.color,
            col3f('whitesmoke'),
            col3f('whitesmoke'),
            col3f('whitesmoke'),
            col3f('#000042'),
        )
        print(m)
        r = self.window.renderer
        self.view = m.view(r)
        self.shader = r.new_shader_program([
            r.new_vertex_shader([vertex_shader]),
            r.new_fragment_shader([fragment_shader]),
        ])
        self.running = False
        self.window.inputs.on_quit.connect(self.stop)
        self.window.inputs.on_resize.connect(self._on_resize)
        self.window.poll()
        self._on_resize(640, 480)

    def _on_resize(self, w, h):
        self.projection_matrix = cube.gl.matrix.perspective(
            45, w / h, 0.005, 300.0
        )
        r = self.window.renderer
        r.clear(cube.gl.BufferBit.color | cube.gl.BufferBit.depth)
        r.viewport(0, 0, w, h)

    def run(self):
        self.running = True
        import time
        start = time.time()
        frames = 0
        while self.running:
            self.window.poll()
            self.window.renderer.clear(cube.gl.BufferBit.color | cube.gl.BufferBit.depth)
            with self.window.renderer.begin(cube.gl.mode_3d) as painter:
                painter.state.projection = self.projection_matrix
                painter.state.view = cube.gl.matrix.look_at(
                    cube.gl.Vector3f(0, 3, 0),
                    cube.gl.Vector3f(0, 0, 0),
                    cube.gl.Vector3f(0, 0, 1),
                )
                with painter.bind([self.shader]):
                    painter.draw([self.view])
            self.window.swap_buffers()
            frames += 1
        print(frames/(time.time() - start), "fps")
    def stop(self):
        self.running = False

app = App()
app.run()
