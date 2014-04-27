
import cubeapp.game

from cube.gui.widgets import Viewport
from cube import scene
from cube import gl
from cube.units import angle
from cube.system.inputs import KeySym

import random

MIN_Y, MAX_Y = -10, 10
MIN_X, MAX_X = -19, 19
NFF_CUBE = """nff
version 2.0

cube               // Name of the object.
8                  // Number of vertices.
0.5 0.5 -0.5       // Vertex info.
0.5 -0.5 -0.5
-0.5 -0.5 -0.5
-0.5 0.5 -0.5
0.5 0.5 0.5
0.5 -0.5 0.5
-0.5 -0.5 0.5
-0.5 0.5 0.5
6                         // Number of polygons.
4 0 1 2 3 0xf00 both      // 0xf00 is the color, in this case, Red
4 7 6 5 4 0x0f0 both      // "both" sides of the cube's faces are visible,
4 0 4 5 1 0x00f both      // this means it is visible even from inside the cube.
4 1 5 6 2 0xff0 both
4 2 6 7 3 0xfff both
4 3 7 4 0 0x0ff both
"""
NFF_SPHERE = "tess 2\ns 0 0 0 1\n"

class ModelView:
    def __init__(self, nff_source, renderer,
                 ambient = gl.Color3f("pink"),
                 diffuse = gl.Color3f("white"),
                 specular = gl.Color3f("white"),
                 shininess = 3,
                 pos = gl.vec3f(0),
                 dir = gl.vec3f(0),
                 scale = gl.vec3f(1),
                ):
        s = scene.from_string(nff_source, "nff")
        mesh = s.meshes[0]
        mat = s.materials[0]
        mat.ambient = ambient
        mat.shading_model = gl.material.ShadingModel.gouraud
        mat.diffuse = diffuse
        mat.specular = specular
        mat.shininess = shininess
        self.mesh_view = mesh.view(renderer)
        self.mat_view = mat.bindable(renderer)
        self.pos = gl.vec3f(pos)
        self.dir = gl.vec3f(dir)
        self.scale = gl.vec3f(scale)

    def render(self, painter):
        painter.push_state()
        painter.state.translate(self.pos)
        painter.state.scale(self.scale)
        with painter.bind([self.mat_view]):
            painter.draw([self.mesh_view])
        painter.pop_state()

    def update(self, delta):
        self.pos += self.dir * delta
        self.check_constraints()

    def check_constraints(self):
        pass

class Pad(ModelView):
    def __init__(self, renderer, pos, name):
        self.name = name
        self.height = 8
        self.width = 1
        super().__init__(
            NFF_CUBE, renderer,
            ambient = gl.Color3f("#01018f"),
            pos = pos,
            scale = gl.vec3f(self.width, self.height, 1),
        )

    def check_constraints(self):
        min_y, max_y = MIN_Y + self.height / 2, MAX_Y - self.height / 2
        if self.pos.y > max_y:
            self.pos.y = max_y
        if self.pos.y < min_y:
            self.pos.y = min_y

from cubeapp.game import Controller
from cubeapp.game.event import Channel, Event

class PadController(Controller):

    def __init__(self, pad):
        self.move_up = Channel(pad.name + '_move_up', dir = 1)
        self.move_down = Channel(pad.name + '_move_down', dir = -1)
        self.stop = Channel(pad.name + '_stop')
        super().__init__(
            target = pad,
            channels = [self.move_up, self.move_down, self.stop],
        )

    def fire(self, ev, delta):
        if ev.channel == self.stop:
            self.target.dir.y = 0
        else:
            self.target.dir.y = ev.channel.dir * 17

class Ball(ModelView):

    def __init__(self, renderer, lpad, rpad, pos, dir):
        self.radius = .5
        self.lpad = lpad
        self.rpad = rpad
        super().__init__(
            NFF_SPHERE, renderer,
            ambient = gl.Color3f("#845412"),
            dir = dir,
            scale = gl.vec3f(self.radius * 2),
            pos = pos,
        )

    def check_constraints(self):
        min_y, max_y = MIN_Y + self.radius, MAX_Y - self.radius
        min_x, max_x = MIN_X + self.radius, MAX_X - self.radius

        if self.pos.y >= max_y:
            self.pos.y = max_y
            self.dir.y = -1 * self.dir.y  +  random.random() - 0.5

        if self.pos.y <= min_y:
            self.pos.y = min_y
            self.dir.y = -1 * self.dir.y  +  random.random() - 0.5

        def collide_with(pad):
            return (
                self.pos.y >= pad.pos.y - pad.height / 2 and
                self.pos.y <= pad.pos.y + pad.height / 2
            )

        lpad = self.lpad
        rpad = self.rpad
        if self.pos.x > max_x:
            self.dir.x *= -1
            self.pos.x = max_x
        elif self.pos.x >= max_x - 2*lpad.width:
            if collide_with(lpad):
                self.pos.x = max_x - 2*lpad.width
                self.dir.x = -1 * self.dir.x  +  random.random() - 0.5

        if self.pos.x < min_x:
            self.dir.x *= -1
            self.pos.x = min_x
        elif self.pos.x <= min_x + 2* rpad.width:
            if collide_with(rpad):
                self.pos.x = min_x + 2*rpad.width
                self.dir.x = -1 * self.dir.x  +  random.random() - 0.5

class GameView(Viewport):

    def __init__(self, game, objects,  **kw):
        super().__init__(id_ = "gameview", **kw)

        self.light = self.renderer.new_light(
            gl.PointLightInfo(
                gl.vec3f(0),
                gl.Color3f("#888"),
                gl.Color3f("#333"),
            )
        )
        self.game = game
        self.objects = objects

    def render(self, ignored_painter):
        with self.renderer.begin(gl.mode_3d) as painter:
            painter.state.look_at(
                gl.vec3f(0, 0, -30), gl.vec3f(0, 0, 0), gl.vec3f(0, 1, 0)
            )
            painter.state.perspective(
                45, self.game.window.width / self.game.window.height, 0.005, 300.0
            )
            with painter.bind([self.light]):
                for obj in self.objects:
                    obj.render(painter)


class Game(cubeapp.game.Game):

    def __init__(self, window):
        self.scene = scene.Scene()
        self.lpad = Pad(window.renderer, gl.vec3f(MAX_X - 1, 0, 0), "left")
        self.rpad = Pad(window.renderer, gl.vec3f(MIN_X + 1, 0, 0), "right")
        self.pads = [self.lpad, self.rpad]
        self.balls = [
            Ball(window.renderer,
                 lpad = self.lpad,
                 rpad = self.rpad,
                 pos = gl.vec3f((random.random() - .5) * 1),
                 dir = gl.vec3f(20, 7, 0) * (random.random()*.5 + .5),
            ) for _ in range(10)
        ]
        self.ball = self.balls[0]
        self.walls = [
            ModelView(
                NFF_CUBE, window.renderer,
                pos = gl.vec3f(pos[0], pos[1], 0),
                scale = gl.vec3f(size[0], size[1], 5),
                ambient = gl.Color3f("darkred")
            ) for pos, size in (
                ((0, MAX_Y + .5), (MAX_X - MIN_X, 1)), # TOP
                ((0, MIN_Y - .5), (MAX_X - MIN_X, 1)), # BOTTOM
                ((MIN_X - .5, 0), (1, MAX_Y - MIN_Y)), # LEFT
                ((MAX_X + .5, 0), (1, MAX_Y - MIN_Y)), # RIGHT
            )
        ]
        self.objects = self.pads + self.balls + self.walls
        super().__init__(
            window,
            view = GameView(
                game = self,
                objects = self.objects,
                renderer = window.renderer,
            ),
            bindings = {
                'keyboard': {
                    'lpad_up': KeySym.w,
                    'lpad_down': KeySym.s,
                    'rpad_up': KeySym.up,
                    'rpad_down': KeySym.down,
                },
            },
        )

        mgr = self.event_manager
        lctrl = mgr.emplace(PadController, self.lpad)
        rctrl = mgr.emplace(PadController, self.rpad)

        kb = self.input_translator.keyboard
        forwards = [
            (kb.lpad_up.key_released, lctrl.stop),
            (kb.lpad_down.key_released, lctrl.stop),
            (kb.lpad_up.key_held, lctrl.move_up),
            (kb.lpad_down.key_held, lctrl.move_down),

            (kb.rpad_up.key_released, rctrl.stop),
            (kb.rpad_down.key_released, rctrl.stop),
            (kb.rpad_up.key_held, rctrl.move_up),
            (kb.rpad_down.key_held, rctrl.move_down),
        ]
        def to_event(slot, channel):
            slot.connect(
                lambda input:self.event_manager.push(Event(channel))
            )
        for slot, channel in forwards:
            to_event(slot, channel)

    def update(self, delta):
        super().update(delta)
        self.view.light.point.position = self.ball.pos
        for obj in self.objects:
            obj.update(delta)



