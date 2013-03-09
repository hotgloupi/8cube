# -*- encoding: utf-8 -*-

from cube.gl import frustum, vec3il, vec3f, Sphereil, vec3d

fov = 45
ratio = 640 / 480
near_dist = 0
far_dist = 300

f = frustum.Frustumil(fov, ratio, near_dist, far_dist)

pos = vec3il(10,12,12)
up = vec3f(0,1,0)
front = vec3f(0,0,-1)
f.update(pos, front, up)

for p in frustum.PlanePosition.values.values():
    assert f.plane(p).distance(vec3d(10, 12, 11)) < 0
    assert f.plane(p).distance(vec3d(10, 10, -100)) < 0

pos = vec3il(10,12,11)

print(f.contains(pos))
assert f.contains(pos)


s = Sphereil(pos, 12)
assert f.intersect(s)

pos = vec3il(10,15,11)
s = Sphereil(pos, 12)
assert f.intersect(s)


pos = vec3il(10,12,-288)
s = Sphereil(pos, 1)
assert f.intersect(s)

pos = vec3il(10,12,-290)
s = Sphereil(pos, 1)
assert not f.intersect(s)


f.update(
    vec3il(0,0,0),
    vec3f(1, 0, 0),
    vec3f(0, 1, 0),
)

s = Sphereil(vec3il(0, 0, 0), 10)
assert f.intersect(s)

s = Sphereil(vec3il(310, 0, 0), 10)
assert f.intersect(s)

s = Sphereil(vec3il(311, 0, 0), 10)
assert not f.intersect(s)
