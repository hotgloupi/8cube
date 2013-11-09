# -*- encoding: utf8 -*-

from cube.gl import frustum, vec3il, vec3f, Sphereil, vec3d, vector
from cube.units import deg

from unittest import TestCase

class _(TestCase):

    def setUp(self):
        fov = deg(45.0)
        ratio = 640 / 480
        near_dist = 1
        far_dist = 300
        self.frustum = frustum.Frustumil(fov, ratio, near_dist, far_dist)
        up = vec3f(0,1,0)
        front = vec3f(0,0,-1)
        self.frustum.update(front, up)

    def test_nil(self):
        pass
    def test_plane_position(self):
        for p in frustum.PlanePosition.values.values():
            self.assertLessEqual(self.frustum.plane(p).distance(vec3d(0, 0, -1)), 0)
            self.assertLessEqual(self.frustum.plane(p).distance(vec3d(10, 10, -100)), 0)
