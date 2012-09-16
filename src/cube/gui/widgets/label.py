# -*- encoding: utf-8 -*-

from .. import widget
from cube import gl

class Label(widget.Widget):

    def __init__(self, text, **kwargs):
        kwargs.setdefault('tag', 'label')
        super(Label, self).__init__(**kwargs)
        self.__str = text
        self.__text = None

    def _prepare(self, renderer):
        self.__sp  = renderer.new_shader_program()
        fs = renderer.new_fragment_shader()
        fs.push_source("""
            uniform sampler2D sampler0;
            void main(void) {
                float c = texture2D(sampler0, vec2(gl_TexCoord[0])).r;
                gl_FragColor = vec4(1,1,1,c);
            }
        """)
        self.__sp.push_shader(fs)
        del fs
        vs = renderer.new_vertex_shader()
        vs.push_source("""
            uniform mat4 cube_ModelViewProjectionMatrix;
            void main(void)
            {
                //gl_FrontColor = gl_Color;
                gl_TexCoord[0] = gl_MultiTexCoord0;
                gl_Position = cube_ModelViewProjectionMatrix * gl_Vertex;
            }
        """)
        self.__sp.push_shader(vs)
        del vs
        self.__sp.finalize()
        self.__sp.parameter("cube_ModelViewProjectionMatrix")
        self.__text = gl.Text(self.font, self.__str);

    def render(self, painter):
        painter.bind(self.__sp)
        painter.draw(self.__text, self.__sp.parameter("sampler0"))
        painter.unbind(self.__sp)

