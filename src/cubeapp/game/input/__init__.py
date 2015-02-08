# -*- encoding: utf-8 -*-

from cube.system.inputs import KeyMod, KeySym
from cube import log

from .keyboard import KeyboardInput
from .mouse import MouseInput

class Translator:
    """Connects to input devices such as keyboard and mouse, and translate them
    to events according to the binding configuration.

    Bindings consist of a dictionnary that contains mapping for each device
    kind:
        >>> bindings = {
        ...     'mouse': {...},
        ...     'keyboard': {...},
        ...     'joystick': {...},
        ... }

    # Keyboard bindings:
        The dictionary describing keyboard bindings contains key of type string
        that will refer to a channel (like `translator.keyboard.move_forward`).
        The values are of the form:
            * A string of length 1 (like 'c').
            * An integer or a cube.system.inputs.KeySym.`sym`.
            * A tuple (sym, mod) where sym is a KeySym and mod a KeyMod.
        For example:
            >>> bindings = {
            ...     'keyboard': {
            ...         'move_forward': KeySym.up,
            ...         'sprint_forward': (KeySym.up, KeyMod.lshift),
            ...     }
            ... }

        The channels created allow user to be notified of keyboard events
        through their method `connect`:
            >>> translator.keyboard.move_forward.connect(
            ...     lambda input: print("got keyboard event", input.name)
            ... )

        See `cubeapp.game.input.KeyboardInput` class for more information on
        the callback parameter.

    """

    def __init__(self, window, bindings):
        self.__keyboard_map = {}
        class Keyboard:
            """Namespace for keyboard connectors"""
            pass
        self.keyboard = Keyboard()
        for name, key in bindings.get('keyboard', {}).items():
            key, mod = self.__translate_key(name, key)
            key_input = KeyboardInput(name)
            log.debug("Map key", mod, key, 'to', name, key_input)
            assert (mod, key) not in self.__keyboard_map
            setattr(self.keyboard, name, key_input)
            if mod == KeyMod.ctrl:
                self.__keyboard_map[(KeyMod.lctrl, key)] = key_input
                self.__keyboard_map[(KeyMod.rctrl, key)] = key_input
            else:
                self.__keyboard_map[(mod, key)] = key_input

        self.mouse = MouseInput()

        self._hdlrs = [
            window.inputs.on_keydown.connect(self.__on_keydown),
            window.inputs.on_keyup.connect(self.__on_keyup),
            window.inputs.on_mousemove.connect(self.__on_mousemove),
            window.inputs.on_mousedown.connect(self.__on_mousedown),
            window.inputs.on_mouseup.connect(self.__on_mouseup),
        ]
        self.__events = []
        self.__held = set()

    def __translate_key(self, name, key):
        mod = KeyMod.none
        if isinstance(key, tuple):
            if len(key) != 2:
                raise Exception((
                    "Cannot bind '{}' with '{}': it should be a tuple "
                    "(KeyMod, key)"
                ).format(name, key))
            return key
        if isinstance(key, KeySym) or isinstance(key, int):
            return key, mod
        elif isinstance(key, str):
            if len(key) != 1:
                raise Exception((
                    "Cannot bind '{}' with '{}': it should be a single "
                    "character"
                ).format(name, key))
            return ord(key), mod
        raise Exception((
            "Cannot bind '{}' with '{}': it should be a single "
            "character, a KeySym, a key code, or a tuple (KeyMod, key)"
        ).format(name, key))

    def __get_keyboard_input(self, keymod, keysym, keycode):
        keymod = KeyMod(keymod & ~KeyMod.num)
        i = self.__keyboard_map.get((keymod, keysym))
        if i is None:
            i = self.__keyboard_map.get((keymod, keycode))
        return i

    def __on_keydown(self, keymod, keysym, keycode):
        i = self.__get_keyboard_input(keymod, keysym, keycode)
        if i is not None:
            self.__events.append(i.key_pressed)
            self.__held.add(i)
        log.debug("Got keyboard input:", keymod, keysym, keycode, "add", i)

    def __on_keyup(self, keymod, keysym, keycode):
        log.debug("Got keyboard input:", keymod, keysym, keycode)
        i = self.__get_keyboard_input(keymod, keysym, keycode)
        if i is not None:
            self.__events.append(i.key_released)
            try:
                self.__held.remove(i)
            except:
                log.warn("Couldn't remove held key", keymod, keysym, keycode, "cleaning up")
                self.__held = set()
        log.debug("Got keyboard input:", keymod, keysym, keycode, "remove", i)

    def __on_mousemove(self, xrel, yrel, keymod):
        # XXX use keymod ?
        self.mouse.xrel += xrel
        self.mouse.yrel += yrel

    def __on_mousedown(self, button, keymod):
        log.debug("Got mouse down event", button, keymod)
        pass

    def __on_mouseup(self, button, keymod):
        log.debug("Got mouse up event", button, keymod)
        pass

    def poll(self):
        to_fire = self.__events
        self.__events = []
        for func in to_fire:
            func()
        for key in self.__held:
            log.debug("Trigger held key", key)
            key.key_held()



