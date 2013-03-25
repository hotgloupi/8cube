# -*- encoding: utf-8 -*-

from cube import gl, units

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
        self.__tree = tree.Tree()
        Chunk.prepare(renderer)
        self.referential = gl.Vector3il()
        self.__nodes_to_render2 = []
        self.__frustum_view = None
        attr = gl.renderer.make_vertex_buffer_attribute
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

    def update(self, delta, player, projection_matrix):
        self.__pos = gl.vec3d(
            player.world_position.x + player.position.x / Chunk.size,
            player.world_position.y + player.position.y / Chunk.size,
            player.world_position.z + player.position.z / Chunk.size,
        )
        print(self.__pos)
        self.__frustum = gl.frustum.Frustumd(units.deg(45), 640.0 / 480.0,0.1,4)
        self.__frustum.update(
            player.camera.front,
            player.camera.up
        )
        self.referential = player.world_position
        self.__nodes_to_render = []
        self.__checked = 0
        self.__tree.visit_nodes(self.__on_tree_node)
        print("Found", len(self.__nodes_to_render), "nodes",
              player.world_position,self.__pos, player.camera.front, player.camera.up, "check=",
             self.__checked)
        #self._fix()

    def __on_tree_node(self, node):
        #if self.__checked > 10000:
        #    print(".", end='')
        #    return tree.VisitAction.stop

        #if  node.origin.y > 0:
        #    return tree.VisitAction.stop

        self.__checked += 1
        if self.__checked > 5000:
            print(".", end='')
            return tree.VisitAction.stop

        if len(self.__nodes_to_render) > 5000:
            return tree.VisitAction.stop_and_clean

        #if node.level > 60 and node.origin.y > 0:


        center = gl.vec3d(
            node.origin.x + node.size / 2,
            node.origin.y + node.size / 2,
            node.origin.z + node.size / 2,
        )
        s = gl.Sphered(center, node.size * 0.7071067811865476)
        if not self.__frustum.intersects(s):
            #print("SKIP", node.level, gl.vector.distance(center, self.__pos) / 2**node.level)
            #print(node.level, node.origin, s)
            return tree.VisitAction.stop_and_clean

        #ratio = gl.vector.distance(center, self.__pos) / node.size
        #if ratio > 1:
        #    self.__nodes_to_render.append(
        #        (node.level, node.origin, self.get_chunk(node.origin))
        #    )
            #return tree.VisitAction.stop
        #print(node.level, node.origin, s, "<-- MATCH")
        if node.level == 0:
            self.__nodes_to_render.append(
                (node.level, node.origin, self.get_chunk(node.origin))
            )
        return tree.VisitAction.continue_

    def _fix(self):
        self.__nodes_to_render2 = self.__nodes_to_render[:]
        self.__frustum_view = self.__frustum.view(self.__renderer)

    def get_chunk(self, pos):
        chunk = self.__storage.get_chunk(pos)
        if chunk is None:
            chunk = self.__generator.gen_chunk(pos)
            self.__storage.set_chunk(pos, chunk)
        return chunk

    def render(self, painter):
        if self.__frustum_view is not None:
            print("Binding sp")
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
                print("Update state")
                Chunk.sp.update(state)
                print("Draw frustum")
                painter.draw([self.__frustum_view])
                print("Done")
        with painter.bind([Chunk.sp, Chunk.vb]):
            ignored = 0
            for level, pos, chunk in self.__nodes_to_render2:
                if level > 0:
                    ignored += 1
                    continue
                try:
                    chunk.render(pos - self.referential, painter)
                except Exception as e:
                    print(e, pos, self.referential, pos-self.referential)
            print(ignored, "ignored node")

