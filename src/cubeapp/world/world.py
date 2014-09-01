# -*- encoding: utf-8 -*-

import threading
import time
import queue

from cube import gl, units, debug
import cube

from .storage import Storage
from .generator import Generator
from . import tree
from .chunk import Chunk

# World coordinate type
coord_type = gl.Vector3il

class World:
    def __init__(self, renderer, storage = Storage(), generator = Generator()):
        assert isinstance(storage, Storage)
        assert isinstance(generator, Generator)
        self.__storage = storage
        self.__generator = generator
        self.__renderer = renderer
        self.__tree_level = 62
        self.__tree = tree.Tree(self.__tree_level)
        Chunk.prepare(renderer)
        self.referential = gl.Vector3il()
        self.__nodes_to_render = []
        self.__nodes_to_render_found = []
        self.__running = False
        self.__thread = None
        self.__nodes = set()
        self.__event_queue = queue.Queue()

    def start(self, camera, ref):
        if self.__running:
            raise Exception("Already running")
        self.update(camera, ref)
        self.__running = True
        self.__thread = threading.Thread(target = self.__run)
        self.__thread.start()

    def update(self, camera, referential):
        self.referential = referential
        self.__position = gl.vec3d(
            referential.x + camera.position.x / Chunk.size,
            referential.y + camera.position.y / Chunk.size,
            referential.z + camera.position.z / Chunk.size,
        )
        self.__frustum = camera.frustum

    def poll(self):
        """Return two set of nodes, those no longer required, those new"""
        to_add = set()
        to_remove = set()
        while not self.__event_queue.empty():
            ev, nodes = self.__event_queue.get_nowait()
            if ev == 'ADD':
                to_add.update(nodes)
            elif ev == 'RM':
                to_remove.update(nodes)
            else:
                raise Exception("Unknown event")
        return (
            [self.get_chunk(n) for n in to_remove],
            [self.get_chunk(n) for n in to_add],
        )

    def __run(self):
        while self.__running:
            time.sleep(0.1)
            nodes = set(tree.find_nodes(
                self.__tree,
                self.__position,
                self.__frustum
            ))
            to_remove = self.__nodes.difference(nodes)
            to_add = nodes.difference(self.__nodes)
            self.__nodes = nodes
            if to_add:
                self.__event_queue.put(("ADD", to_add))
            if to_remove:
                self.__event_queue.put(("RM", to_remove))

    def stop(self):
        if not self.__running:
            raise Exception("Not running")
        self.__running = False
        self.__thread.join()

    def get_chunk(self, node):
        chunk = self.__storage.get_chunk(node)
        if chunk is None:
            chunk = self.__generator.gen_chunk(node)
            self.__storage.set_chunk(node, chunk)
        return chunk

    #def _render(self, painter):
    #    with cube.performance_section("app.WorldRender"):
    #        self._render(painter)

    @cube.check_performance("app.WorldRender")
    def render(self, painter):
        #if self.__frustum_view is not None:
        #    assert False
        #    with painter.bind([Chunk.sp, self.__frustum_colors_vb]):
        #        state = painter.push()
        #        state.translate(
        #            -self.referential.x * Chunk.size,
        #            -self.referential.y * Chunk.size,
        #            -self.referential.z * Chunk.size,
        #        )
        #        state.scale(
        #            Chunk.size, Chunk.size, Chunk.size
        #        )
        #        Chunk.sp.update(state)
        #        painter.draw([self.__frustum_view])

        if self.__nodes_to_render != self.__nodes_to_render_found:
            self.__nodes_to_render = self.__nodes_to_render_found[:]

        with painter.bind([Chunk.vb]):
            ignored = 0
            for node in self.__nodes_to_render:
                if node.level > 0:
                    ignored += 1
                    continue
                try:
                    self.get_chunk(node.origin).render(node.origin - self.referential, painter)
                except Exception as e:
                    print(e, node.origin, self.referential, node.origin-self.referential)

