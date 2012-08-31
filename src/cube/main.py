# -*- encoding: utf-8 -*-

from datetime import datetime

from cube.gui.application import Application

def console():
    import cube
    locals_ = {
        'cube': cube,
    }
    import code
    code.InteractiveConsole(locals_).interact(
        banner="Type 'help(cube)' to get started."
    )

def game():
    app = Application()
    app.run()
    app.shutdown()


def test_vertex_buffer():
    import time
    import cube.gl as gl
    import cube.system as sys
    win = sys.Window("test_vertex_buffer", 640, 480)
    vb = win.renderer.new_vertex_buffer()

    vb.push_static_content(
        gl.ContentKind.vertex,
        list(gl.Vector2f(*v) for v in [
            (10, 10), (630, 10), (630, 470), (10, 470)
        ])
    )

    vb.push_static_content(
        gl.ContentKind.color,
        [
            gl.Color3f('red'),
            gl.Color3f('green'),
            gl.Color3f('gray'),
            gl.Color3f('white'),
        ]
    )
    vb.finalize()

    indices = win.renderer.new_index_buffer()
    indices.push_static_content(
        gl.ContentKind.index,
        [ 0, 1, 2, 3]
    )
    indices.finalize()

    sp  = win.renderer.new_shader_program()
    fs = win.renderer.new_fragment_shader()
    fs.push_source("""
        void main(void)
        {
            gl_FragColor = gl_Color;
        }
    """)
    sp.push_shader(fs)
    vs = win.renderer.new_vertex_shader()
    vs.push_source("""
        uniform mat4 cube_ModelViewProjectionMatrix;
        void main(void)
        {
           gl_FrontColor = gl_Color;
           gl_Position = cube_ModelViewProjectionMatrix * gl_Vertex;
        }
    """)
    sp.push_shader(vs)
    sp.finalize()
    with win.renderer.begin(gl.renderer.mode_2d) as painter:
        painter.bind(sp)
        sp.parameter("cube_ModelViewProjectionMatrix")
        win.renderer.viewport(0,0,640,480)
    win.renderer.swap_buffers()
    time.sleep(1)

    print("Running")
    state = {
        'running': True,
    }
    win.poll()
    win.renderer.clear(
        gl.renderer.BufferBit.color |
        gl.renderer.BufferBit.depth
    );
    win.inputs.on_quit.connect(lambda: state.update({'running': False}))
    def f(w, h):
        print("Resize")
        win.renderer.viewport(0, 0, w, h)
    win.inputs.on_expose.connect(f)
    win.renderer.swap_buffers()
    while state['running'] is True:
        win.renderer.clear(
            gl.renderer.BufferBit.color |
            gl.renderer.BufferBit.depth
        );

        with win.renderer.begin(gl.renderer.mode_2d) as painter:
            painter.bind(sp)
            painter.bind(vb)
            win.poll()
            painter.draw_elements(gl.DrawMode.quads, indices, 0, 4)

        win.renderer.swap_buffers()
        time.sleep(.5)

    pass

import cube.gl.test
def main(args):
    #return test_vertex_buffer()
    #return cube.gl.test.test_all()
    print(args)
    if not args:
        return game()
    eval(args[0] + "()")
