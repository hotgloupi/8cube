
from cube import gl

import threading
import math

class Renderer:

    def __init__(self, renderer):
        self.renderer = renderer
        self.__materials = {}
        for lod in range(8):
            mat = gl.Material('ground-%s' % lod)
            mat.ambient = gl.col3f('#' + ('%s' % (lod + 1)) * 3)
            self.__materials[lod] = mat.bindable(renderer)
        mesh = gl.Mesh()
        mesh.mode = gl.DrawMode.quads
        size = 16
        step = 1.0 / size
        for i in (i / size for i in range(size)):
            x = i
            for j in (i / size for i in range(size)):
                y = 1.0 - j
                mesh.kind = gl.ContentKind.vertex
                mesh.append(gl.vec3f(x, 0, y - step))
                mesh.append(gl.vec3f(x, 0, y))
                mesh.append(gl.vec3f(x + step, 0, y))
                mesh.append(gl.vec3f(x + step, 0, y - step))
                mesh.kind = gl.ContentKind.tex_coord0
                mesh.append(gl.vec2f(i, j + step))
                mesh.append(gl.vec2f(i + step, j + step))
                mesh.append(gl.vec2f(i + step, j))
                mesh.append(gl.vec2f(i, j))
                mesh.kind = gl.ContentKind.normal
                mesh.append(gl.vec3f(0, 1, 0))
                mesh.append(gl.vec3f(0, 1, 0))
                mesh.append(gl.vec3f(0, 1, 0))
                mesh.append(gl.vec3f(0, 1, 0))
        self.mesh = mesh.drawable(self.renderer)
        self.__chunks = {}
        self.__chunks_lock = threading.Lock()

    def add_chunk(self, chunk):
        with self.__chunks_lock:
            self.__chunks.setdefault(chunk.node.size, set()).add(chunk)


    def remove_chunk(self, chunk):
        with self.__chunks_lock:
            self.__chunks.setdefault(chunk.node.size, set()).remove(chunk)

    def render(self, referential, painter):
        with self.__chunks_lock:
            state = painter.push_state()
            state.render_state(gl.RenderState.depth_test, False)
            keys = reversed(sorted(self.__chunks.keys()))
            for k in keys:
                lod = int(math.log(k, 2))
                if lod not in self.__materials:
                    continue
                self.__render_size(
                    k,
                    self.__materials[lod],
                    self.__chunks[k],
                    referential,
                    painter,
                    state
                )
            painter.pop_state()

    def __render_size(self, size, material, chunks, referential, painter, state):
        for chunk in chunks:
            origin = chunk.node.origin - referential
            if origin.y != 0: continue
            state.model = gl.matrix.scale(
                gl.matrix.translate(
                    gl.mat4f(),
                    gl.vec3f(
                        origin.x * chunk.size,
                        origin.y * chunk.size,
                        origin.z * chunk.size
                    )
                )
                , gl.vec3f(chunk.node.size * chunk.size)
            )
            with painter.bind([material]):
                painter.draw([self.mesh])
