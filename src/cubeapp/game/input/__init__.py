# -*- encoding: utf-8 -*-

from cube.system.inputs import KeyMod, KeySym

from .keyboard import KeyboardInput
from .mouse import MouseInput

class Translator:
    """Provide signals and state for given bindings.
    """

    def __init__(self, window, bindings):
        self.__map = {}
        for name, key in bindings.items():
            key_input = KeyboardInput(name)
            key, mod = self.__translate_key(name, key)
            setattr(self, name, key_input)
            self.__map[(mod, key)] = key_input

        self.mouse = MouseInput()

        self._hdlrs = [
            window.inputs.on_keydown.connect(self.__on_keydown),
            window.inputs.on_keyup.connect(self.__on_keyup),
            window.inputs.on_mousemove.connect(self.__on_mousemove),
        ]

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
        i = self.__map.get((keymod, keysym))
        if i is None:
            i = self.__map.get((keymod, keycode))
        return i

    def __on_keydown(self, keymod, keysym, keycode):
        i = self.__get_keyboard_input(keymod, keysym, keycode)
        if i is not None:
            i.keydown()

    def __on_keyup(self, keymod, keysym, keycode):
        i = self.__get_keyboard_input(keymod, keysym, keycode)
        if i is not None:
            i.keyup()

    def __on_mousemove(self, xrel, yrel):
        self.mouse.xrel += xrel
        self.mouse.yrel += yrel
