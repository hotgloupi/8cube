
from .VertexBufferAttribute import *
from .constants import ContentKind, ContentHint
from cube.gl.color import Color3f, Color4f
from .. import vec2f, vec3f, vec4f
from cube.test import Case

class _(Case):

    content_hints = [
        ContentHint.stream_content,
        ContentHint.dynamic_content,
        ContentHint.static_content,
    ]

    contents = {
        ContentKind.vertex: (
            [vec2f(1,2), vec2f(2,3), vec2f(4,5), vec2f(6,7)],
            [vec3f(1,2,3), vec3f(2,3,4), vec3f(4,5,6), vec3f(5,6,7)],
            [vec4f(1,2,3,4), vec4f(2,3,4,5), vec4f(4,5,6,7), vec4f(5,6,7,8)],
        ),
        ContentKind.color: (
            [Color3f(1,1,1), Color3f(1,1,1), Color3f(1,1,1), Color3f(1,1,1),],
            [Color4f(1,1,1), Color4f(1,1,1), Color4f(1,1,1), Color4f(1,1,1),],
        ),
        ContentKind.index: (
            [1,2,3,4,5],
        ),
        ContentKind.normal: (
            [vec3f(1,2,3), vec3f(2,3,4), vec3f(4,5,6), vec3f(5,6,7)],
            [vec4f(1,2,3,4), vec4f(2,3,4,5), vec4f(4,5,6,7), vec4f(5,6,7,8)],
        ),
        ContentKind.tex_coord0: (
            [vec2f(1,2), vec2f(2,3), vec2f(4,5), vec2f(6,7)],
        ),
        ContentKind.tex_coord1: (
            [vec2f(1,2), vec2f(2,3), vec2f(4,5), vec2f(6,7)],
        ),
        ContentKind.tex_coord2: (
            [vec2f(1,2), vec2f(2,3), vec2f(4,5), vec2f(6,7)],
        ),
    }

    def test_basic(self):
        for hint in self.content_hints:
            for kind, tests in self.contents.items():
                for test in tests:
                    vba = make_vertex_buffer_attribute(kind, test, hint)
                    self.assertIsNotNone(vba)
                    del vba
                    import gc
                    gc.collect()
