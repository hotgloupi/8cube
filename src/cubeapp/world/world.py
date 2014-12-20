# -*- encoding: utf-8 -*-

import threading
import time
import queue

from cube import gl, units, debug, scene
import cube

from .storage import Storage
from .generator import Generator
from . import tree
from .chunk import Chunk
from .renderer import Renderer

# World coordinate type
coord_type = gl.Vector3il

class World:
    def __init__(self,
                 storage,
                 generator,
                 renderer):
        assert isinstance(storage, Storage)
        assert isinstance(generator, Generator)
        assert isinstance(renderer, Renderer)
        self.__renderer = renderer
        self.__storage = storage
        self.__generator = generator
        self.__tree_level = 62
        self.__tree = tree.Tree(self.__tree_level)
        self.__referential = gl.Vector3il()
        self.__running = False
        self.__thread = None
        self.__nodes = set()

    def start(self, camera, ref):
        if self.__running:
            raise Exception("Already running")
        self.update(camera, ref)
        self.__running = True
        self.__thread = threading.Thread(target = self.__run)
        self.__thread.start()

    def update(self, camera, referential):
        self.__referential = tree.Node(referential, 1)
        self.__frustum = camera.frustum

    def __run(self):
        while self.__running:
            nodes = set(tree.find_close_nodes(
                self.__referential,
            ))
            to_add = nodes.difference(self.__nodes)
            to_remove = self.__nodes.difference(nodes)
            if to_add:
                self.__nodes = nodes # .update(to_add)
                self.__renderer.remove_chunks(self.__get_chunk(node) for node in to_remove)
                self.__renderer.add_chunks(self.__get_chunk(node) for node in to_add)
            else:
                time.sleep(.5)

    def stop(self):
        if not self.__running:
            return
        self.__running = False
        self.__thread.join()

    def __get_chunk(self, node):
        chunk = self.__storage.get_chunk(node)
        if chunk is None:
            chunk = self.__generator.gen_chunk(node)
            self.__storage.set_chunk(node, chunk)
        return chunk

    def render(self, painter):
        self.__renderer.render(self.__referential.origin, painter)
        return

