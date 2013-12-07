# -*- encoding: utf-8 -*-

from .. import view
from cube import gl

class Label(view.View):

    def __init__(self, text, **kwargs):
        kwargs.setdefault('tag', 'label')
        super(Label, self).__init__(**kwargs)
        self.__str = text
        self.__text = None

    def _prepare(self, renderer):
        self.__sp  = renderer.new_shader_program([
            renderer.new_fragment_shader([
                """
            uniform sampler2D sampler0;
            void main(void) {
                float c = texture2D(sampler0, vec2(gl_TexCoord[0])).r;
                if (c == 0.0) discard;
                gl_FragColor = vec4(vec3(.1,.1,.1) , c);
            }
        """]),
        renderer.new_vertex_shader(["""
            uniform mat4 cube_ModelViewProjectionMatrix;
            void main(void)
            {
                //gl_FrontColor = gl_Color;
                gl_TexCoord[0] = gl_MultiTexCoord0;
                gl_Position = cube_ModelViewProjectionMatrix * gl_Vertex;
            }
        """]),
        ])
        self.__text = gl.Text(self.font, self.__str);

    @property
    def text(self):
        return self.__str

    @text.setter
    def text(self, value):
        self.__str = value
        self.__text = gl.Text(self.font, self.__str);

    def render(self, painter):
        self.__sp['cube_ModelViewProjectionMatrix'] = painter.state.mvp
        with painter.bind([self.__sp]):
            self.__text.draw(painter, self.__sp["sampler0"])

