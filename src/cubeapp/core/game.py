# -*- encoding: utf-8 -*-

from cube import gl

from .inputs import Inputs

def load_game(games_path, name, window, client):
    import sys
    old_python_path = sys.path
    sys.path = [games_path] + sys.path
    try:
        game_module = __import__(name + '.game', fromlist=['game'])
    finally:
        sys.path = old_python_path
    return game_module.Game(window, client)


class Game():
    """Base class for every games
    """

    def __init__(self, window, client, bindings):
        print("core.Game(", window, client, bindings,")")
        self.window = window
        self.renderer = window.renderer
        self.client = client
        self._prepare()
        self.inputs = Inputs(window, bindings)

    @property
    def gui(self):
        """Game menu GUI"""
        raise Exception("Not implemented")

    @property
    def player(self):
        raise Exception("Not implemented")

    def _prepare(self):
        assert self.renderer is not None
        r = self.renderer
        self.__vb = r.new_vertex_buffer()
        w = 5
        h = 5
        self.__vb.push_static_content(
            gl.ContentKind.vertex,
            list(
                gl.Vector3f(*v) for v in [
                    (0, 0, -10),
                    (w, 0, -10),
                    (w, h, -10),
                    (0, h, -10),
                ]
            )
        )
        self.__vb.push_static_content(
            gl.ContentKind.color,
            [
                gl.Color3f('orange'),
                gl.Color3f('green'),
                gl.Color3f('gray'),
                gl.Color3f('blue'),
            ]
        )
        self.__vb.finalize()
        self.__indices = r.new_index_buffer()
        self.__indices.push_static_content(
            gl.ContentKind.index,
            [ 0, 1, 2, 3]
        )
        self.__indices.finalize()

        self.__sp  = r.new_shader_program()
        fs = r.new_fragment_shader()
        fs.push_source("""
            void main(void)
            {
                gl_FragColor = gl_Color;
            }
        """)
        self.__sp.push_shader(fs)
        del fs
        vs = r.new_vertex_shader()
        vs.push_source("""
            uniform mat4 cube_ModelViewProjectionMatrix;
            void main(void)
            {
               gl_FrontColor = gl_Color;
               gl_Position = cube_ModelViewProjectionMatrix * gl_Vertex;
            }
        """)
        self.__sp.push_shader(vs)
        del vs
        self.__sp.finalize()
        self.__sp.parameter("cube_ModelViewProjectionMatrix")
        self.on_resize(self.window.size.x, self.window.size.y)

    def on_resize(self, w, h):
        self.__projection_matrix = gl.matrix.perspective(
            90, w / h, 0.005, 300.0
        )

    def update(self, delta):
        """Update the game state according to the delta (in seconds).
        should be overridden
        """
        self.player.update(delta)

