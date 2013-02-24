# -*- encoding: utf-8 -*-

from cube.gl import frustum, vec3il, vec3f

fov = 45
ratio = 640 / 480
near_dist = 0
far_dist = 300

f = frustum.Frustumil(fov, ratio, near_dist, far_dist)

pos = vec3il(10,12,12)
up = vec3f(0,1,0)
front = vec3f(0,0,-1)
f.update(pos, front, up)

pos = vec3il(10,12,11)
for p in frustum.PlanePosition.values.values():
    print(p, f.plane(p).distance(pos))

print(f.contains(pos))
print(pos, f.contains(pos))
