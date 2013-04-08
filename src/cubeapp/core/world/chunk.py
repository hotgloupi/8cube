# -*- encoding: utf-8 -*-

from cube import gl
import cube

class Chunk:

    size = 16

    @classmethod
    def prepare(cls, renderer):
        print("PREPARE")
        r = renderer
        w = cls.size
        h = cls.size
        attr = gl.renderer.make_vertex_buffer_attribute
        cls.vb = r.new_vertex_buffer([
            attr(
                gl.ContentKind.vertex,
                list(
                    gl.Vector3f(*v) for v in [
                        (0, 0, 0),
                        (w, 0, 0),
                        (w, 0, h),
                        (0, 0, h),
                    ]
                ),
                gl.ContentHint.static_content
            ),
                attr(
                gl.ContentKind.color,
                [
                    gl.Color3f('orange'),
                    gl.Color3f('green'),
                    gl.Color3f('gray'),
                    gl.Color3f('blue'),
                ],
                gl.ContentHint.static_content
            )
        ])
        cls.__indices = r.new_index_buffer(
            attr(
                gl.ContentKind.index,
                [0, 1, 2, 3],
                gl.ContentHint.static_content
            )
        )

        cls.sp  = r.new_shader_program([
            r.new_fragment_shader(["""
                void main(void)
                {
                    gl_FragColor = gl_Color;
                }
            """]),
            r.new_vertex_shader(["""
                uniform vec3 color;
                uniform mat4 cube_ModelViewProjectionMatrix;
                void main(void)
                {
                   gl_FrontColor =  vec4(gl_Color.rgb, 1);
                   gl_Position = cube_ModelViewProjectionMatrix * gl_Vertex;
                }
            """]),
        ])
        #cls.color = cls.sp.parameter("color")
        #color = gl.vec3f(1, 1, 1) #* 16.0 / cls.size


    #def _render(self, pos, painter):
    #    with cube.performance_section("app.ChunkRender"):
    #        self._render(pos, painter)

    @cube.check_performance("app.ChunckUpdateState")
    def _updated_state(self, pos, painter):
        state = gl.State(painter.state)
        return state

    @cube.check_performance("app.ChunckUpdateShader")
    def _update_shader(self, state):
        self.sp.update(state)

    @cube.check_performance("app.ChunkUpdate")
    def _update(self, pos, painter):
        state = self._updated_state(pos, painter)
        state.model = gl.matrix.translate(
            state.model,
            gl.Vector3f(pos.x * self.size, pos.y * self.size, pos.z * self.size)
        )
        self._update_shader(state)

    @cube.check_performance("app.ChunkRender")
    def render(self, pos, painter):
        self._update(pos, painter)
        #color = gl.vec3f(1, 1, 1) # * 16.0 / cls.size
        self._draw(painter)

    @cube.check_performance("app.ChunkDraw")
    def _draw(self, painter):
        painter.draw_elements(gl.DrawMode.quads, self.__indices, 0, 4)
