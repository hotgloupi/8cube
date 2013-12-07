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
        attr = gl.make_vba
        cls.vb = r.new_vertex_buffer([
            attr(
                gl.ContentKind.vertex,
                list(
                    gl.Vector3f(*v) for v in [
                        (0, 0, 0),
                        (0, 0, h),
                        (w, 0, h),
                        (w, 0, 0),
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

        cls.material = gl.Material()
        cls.material.ambient = gl.Color3f("#222")
        cls.material.add_color(
            gl.ShaderParameterType.vec3,
            gl.StackOperation.add
        )
        cls.material_view = cls.material.bindable(r)

    @cube.check_performance("app.ChunkRender")
    def render(self, pos, painter):
        state = painter.push_state().translate(
            pos.x * self.size, pos.y * self.size, pos.z * self.size
        )
        with painter.bind([self.material_view]):
            painter.draw_elements(gl.DrawMode.quads, self.__indices, 0, 4)
        painter.pop_state()

