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
        list(gl.Vector2f(*v) for v in [(0, 0), (1, 0), (1, 1), (0, 1)])
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

    indices = win.renderer.new_vertex_buffer()
    indices.push_static_content(
        gl.ContentKind.index,
        [ 0, 1, 2, 3]
    )
    indices.refresh()

    state = {
        'running': True,
    }
    win.inputs.on_quit.connect(lambda: state.update({'running': False}))
    while state['running'] is True:
        win.poll()
        with win.renderer.begin(gl.renderer.mode_2d) as painter:
            painter.bind(indices)
            painter.bind(vb)
            painter.draw_elements(gl.DrawMode.quads,indices, 0, 4)

        time.sleep(.1)

    pass

def main(args):
    print(args)
    if not args:
        return game()
    eval(args[0] + "()")
