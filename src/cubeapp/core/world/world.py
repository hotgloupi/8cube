# -*- encoding: utf-8 -*-

import threading
import time

from cube import gl, units, debug
import cube

from .storage import Storage
from .generator import Generator
from . import tree
from .chunk import Chunk

# World coordinate type
coord_type = gl.Vector3il

class World:
    def __init__(self, storage, generator, renderer):
        assert isinstance(storage, Storage)
        assert isinstance(generator, Generator)
        self.__storage = storage
        self.__generator = generator
        self.__renderer = renderer
        self.__tree_level = 62
        self.__tree = tree.Tree(self.__tree_level)
        Chunk.prepare(renderer)
        self.referential = gl.Vector3il()
        self.__frustum_view = None
        attr = gl.renderer.make_vertex_buffer_attribute
        self.__frustum = gl.frustum.Frustumd(units.deg(45), 640.0 / 480.0,0.1,40)
        self.__frustum_colors_vb = renderer.new_vertex_buffer([
            attr(
                gl.ContentKind.color,
                [
                    gl.Color3f('#300'),
                    gl.Color3f('#300'),
                    gl.Color3f('#300'),
                    gl.Color3f('#300'),

                    gl.Color3f('#030'),
                    gl.Color3f('#030'),
                    gl.Color3f('#030'),
                    gl.Color3f('#030'),

                    gl.Color3f('#083'),
                    gl.Color3f('#083'),
                    gl.Color3f('#083'),
                    gl.Color3f('#083'),

                    gl.Color3f('violet'),
                    gl.Color3f('violet'),
                    gl.Color3f('violet'),
                    gl.Color3f('violet'),

                    gl.Color3f('purple'),
                    gl.Color3f('purple'),
                    gl.Color3f('purple'),
                    gl.Color3f('purple'),

                    gl.Color3f('#840'),
                    gl.Color3f('#840'),
                    gl.Color3f('#840'),
                    gl.Color3f('#840'),
                ],
                gl.ContentHint.static_content
            )
        ])
        self.__nodes_to_render = []
        self.__nodes_to_render_found = []
        #self.__frustum_view = self.__frustum.view(self.__renderer)

    def _update(self, delta, player, projection_matrix):
        with cube.performance_section("app.WorldUpdate"):
            pass
    def update(self, delta, player, projection_matrix):
        self.__player = player
        self.__pos = gl.vec3d(
            player.world_position.x + player.position.x / Chunk.size,
            player.world_position.y + player.position.y / Chunk.size,
            player.world_position.z + player.position.z / Chunk.size,
        )
        self.__frustum.update(
            self.__player.camera.front,
            self.__player.camera.up
        )
        self.referential = self.__player.world_position

        if not hasattr(self, 'tree_thread'):
            self.tree_thread = threading.Thread(target=self._update_nodes)
            self.tree_thread.start()

    def _update_nodes(self):
        self._search_nodes = True
        while self._search_nodes:
            self.__nodes_to_render_found = list(n for n in tree.find_nodes(
                self.__tree,
                self.__pos,
                self.__frustum
            ) if n.origin.y == -1)
            time.sleep(.1)
            #self.__checked = 0
            #self.__tree.visit(self.__on_tree_node)
            print("Found", len(self.__nodes_to_render_found), "nodes")#,  self.__checked, "checked")

    def stop(self):
        print("Stopping world")
        self._search_nodes = False
        self.tree_thread.join()

    def get_chunk(self, pos):
        chunk = self.__storage.get_chunk(pos)
        if chunk is None:
            chunk = self.__generator.gen_chunk(pos)
            self.__storage.set_chunk(pos, chunk)
        return chunk

    #def _render(self, painter):
    #    with cube.performance_section("app.WorldRender"):
    #        self._render(painter)

    @cube.check_performance("app.WorldRender")
    def render(self, painter):
        if self.__frustum_view is not None:
            assert False
            with painter.bind([Chunk.sp, self.__frustum_colors_vb]):
                state = gl.State(painter.state)
                state.model = gl.matrix.translate(
                    state.model,
                    -self.referential.x * Chunk.size,
                    -self.referential.y * Chunk.size,
                    -self.referential.z * Chunk.size,
                )
                state.model = gl.matrix.scale(
                    state.model,
                    Chunk.size, Chunk.size, Chunk.size
                )
                Chunk.sp.update(state)
                painter.draw([self.__frustum_view])

        if self.__nodes_to_render != self.__nodes_to_render_found:
            self.__nodes_to_render = self.__nodes_to_render_found[:]

        with painter.bind([Chunk.sp, Chunk.vb]):
            ignored = 0
            for node in self.__nodes_to_render:
                if node.level > 0:
                    ignored += 1
                    continue
                try:
                    self.get_chunk(node.origin).render(node.origin - self.referential, painter)
                except Exception as e:
                    print(e, node.origin, self.referential, node.origin-self.referential)

