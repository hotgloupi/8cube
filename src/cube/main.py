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
        list(gl.Vector2f(*v) for v in [(0.1, .1), (.9, .1), (.9, .9), (.1, .9)])
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
    vb.refresh()

    indices = win.renderer.new_index_buffer()
    indices.push_static_content(
        gl.ContentKind.index,
        [ 3,2,1,0]
    )
    indices.refresh()

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
        print (w, h)
        win.renderer.viewport(0, 0, w, h)
    win.inputs.on_expose.connect(f)
    win.renderer.swap_buffers()
    while state['running'] is True:
        win.poll()
        win.renderer.clear(
            gl.renderer.BufferBit.color |
            gl.renderer.BufferBit.depth
        );
        with win.renderer.begin(gl.renderer.mode_3d) as painter:
            painter.bind(vb)
            painter.draw_elements(gl.DrawMode.quads,indices, 0, 4)

        win.renderer.swap_buffers()
        time.sleep(.1)

    pass

def main(args):
    print(args)
    if not args:
        return game()
    eval(args[0] + "()")
