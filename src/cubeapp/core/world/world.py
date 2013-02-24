# -*- encoding: utf-8 -*-

from cube import gl

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

    def update(self, delta, player, projection_matrix):
        self.__frustum = gl.frustum.Frustumil(45, 480.0/640.0,1,30)
        self.__frustum.update(player.world_position, player.camera.front, player.camera.up)
        self.referential = player.world_position
        self.__nodes_to_render = []
        self.__tree.visit_nodes(self.__on_tree_node)
        print("Found", len(self.__nodes_to_render), "nodes")

    def __on_tree_node(self, node):
        if len(self.__nodes_to_render) > 200:
            return tree.VisitAction.stop_and_clean

        center = gl.vec3il(
            node.origin.x + node.size / 2,
            node.origin.y + node.size / 2,
            node.origin.z + node.size / 2,
        )
        s = gl.Sphereil(center, int(node.size / 4))
        if self.__frustum.intersect(s):
            if node.level>3:
                print('found', node.level, node.origin)
            if node.level == 0:
                print("Found", node)
                self.__nodes_to_render.append(
                    (node.origin, self.get_chunk(node.origin))
                )
            return tree.VisitAction.continue_

        return tree.VisitAction.stop_and_clean

    def _fix(self):
        self.__nodes_to_render2 = self.__nodes_to_render[:]
    def get_chunk(self, pos):
        chunk = self.__storage.get_chunk(pos)
        if chunk is None:
            chunk = self.__generator.gen_chunk(pos)
            self.__storage.set_chunk(pos, chunk)
        return chunk

    def render(self, painter):
        with painter.bind([Chunk.sp, Chunk.vb]):
            for pos, chunk in self.__nodes_to_render2:
                chunk.render(pos - self.referential, painter)

    @staticmethod
    def _compute_frustum_planes(mvp_):
        from math import sqrt
        planes = (
            gl.Vector4f(),
            gl.Vector4f(),
            gl.Vector4f(),
            gl.Vector4f(),
            gl.Vector4f(),
            gl.Vector4f(),
        )
        mvp = (
            mvp_[0][0],
            mvp_[0][1],
            mvp_[0][2],
            mvp_[0][3],
            mvp_[1][0],
            mvp_[1][1],
            mvp_[1][2],
            mvp_[1][3],
            mvp_[2][0],
            mvp_[2][1],
            mvp_[2][2],
            mvp_[2][3],
            mvp_[3][0],
            mvp_[3][1],
            mvp_[3][2],
            mvp_[3][3],
        )
        # Right plane
        planes[0][0] = mvp[ 3] - mvp[ 0];
        planes[0][1] = mvp[ 7] - mvp[ 4];
        planes[0][2] = mvp[11] - mvp[ 8];
        planes[0][3] = mvp[15] - mvp[12];

        t = sqrt( planes[0][0] * planes[0][0] + planes[0][1] * planes[0][1] + planes[0][2]    * planes[0][2] );
        planes[0][0] /= t;
        planes[0][1] /= t;
        planes[0][2] /= t;
        planes[0][3] /= t;

        # Extract the numbers for the LEFT plane */
        planes[1][0] = mvp[ 3] + mvp[ 0];
        planes[1][1] = mvp[ 7] + mvp[ 4];
        planes[1][2] = mvp[11] + mvp[ 8];
        planes[1][3] = mvp[15] + mvp[12];
        # Normalize the result */
        t = sqrt( planes[1][0] * planes[1][0] + planes[1][1] * planes[1][1] + planes[1][2]    * planes[1][2] );
        planes[1][0] /= t;
        planes[1][1] /= t;
        planes[1][2] /= t;
        planes[1][3] /= t;
        # Extract the BOTTOM plane */
        planes[2][0] = mvp[ 3] + mvp[ 1];
        planes[2][1] = mvp[ 7] + mvp[ 5];
        planes[2][2] = mvp[11] + mvp[ 9];
        planes[2][3] = mvp[15] + mvp[13];
        # Normalize the result */
        t = sqrt( planes[2][0] * planes[2][0] + planes[2][1] * planes[2][1] + planes[2][2]    * planes[2][2] );
        planes[2][0] /= t;
        planes[2][1] /= t;
        planes[2][2] /= t;
        planes[2][3] /= t;
        # Extract the TOP plane */
        planes[3][0] = mvp[ 3] - mvp[ 1];
        planes[3][1] = mvp[ 7] - mvp[ 5];
        planes[3][2] = mvp[11] - mvp[ 9];
        planes[3][3] = mvp[15] - mvp[13];
        # Normalize the result */
        t = sqrt( planes[3][0] * planes[3][0] + planes[3][1] * planes[3][1] + planes[3][2]    * planes[3][2] );
        planes[3][0] /= t;
        planes[3][1] /= t;
        planes[3][2] /= t;
        planes[3][3] /= t;
        # Extract the FAR plane */
        planes[4][0] = mvp[ 3] - mvp[ 2];
        planes[4][1] = mvp[ 7] - mvp[ 6];
        planes[4][2] = mvp[11] - mvp[10];
        planes[4][3] = mvp[15] - mvp[14];
        # Normalize the result */
        t = sqrt( planes[4][0] * planes[4][0] + planes[4][1] * planes[4][1] + planes[4][2]    * planes[4][2] );
        planes[4][0] /= t;
        planes[4][1] /= t;
        planes[4][2] /= t;
        planes[4][3] /= t;
        # Extract the NEAR plane */
        planes[5][0] = mvp[ 3] + mvp[ 2];
        planes[5][1] = mvp[ 7] + mvp[ 6];
        planes[5][2] = mvp[11] + mvp[10];
        planes[5][3] = mvp[15] + mvp[14];
        # Normalize the result */
        t = sqrt( planes[5][0] * planes[5][0] + planes[5][1] * planes[5][1] + planes[5][2]    * planes[5][2] );
        planes[5][0] /= t;
        planes[5][1] /= t;
        planes[5][2] /= t;
        planes[5][3] /= t;

        return planes

    def _sphere_in_frustum(self, x, y, z, radius, frustum_planes):
        for p in range(6):
            d = frustum_planes[p][0] * x + frustum_planes[p][1] * y + frustum_planes[p][2] * z + frustum_planes[p][3]
            if d <= -radius:
                return 0
        return d + radius
