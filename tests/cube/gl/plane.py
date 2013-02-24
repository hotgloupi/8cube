# -*- encoding: utf-8 -*-

import cube

from cube.gl import Planef, vec3f

p = Planef()
print(p)

p = Planef(vec3f(0,0,0), vec3f(0,1,0))
assert p.normalized
print(p, p.normal, p.coef)
p = Planef(vec3f(1,0,0),vec3f(0,0,0), vec3f(0,0,1))
print(p, p.normal, p.coef)

print(p.distance(vec3f(0,10,0)))
print(p.raw_distance(vec3f(0,10,0)))
