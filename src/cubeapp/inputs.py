# -*- encoding: utf-8 -*-

from cube.system.inputs import KeyMod, KeySym


class Connector:
    class Handler:
        def __init__(self, connector):
            self.__connector = connector

        def disconnect(self):
            self.__connector.disconnect()

    def __init__(self, input_):
        self.__input = input_
        self.__callbacks = []

    def connect(self, cb):
        hdlr = Handler(self)
        self.__callbacks.append((hdlr, cb))
        return hdlr

    def disconnect(self, hdlr):
        idx = None
        for i, pair in enumerate(self.__callbacks):
            if pair[0] == hdlr:
                idx = i
                break
        if idx is None:
            raise KeyError("Couldn't find this handler")
        self.__callbacks.pop(idx)

    def __call__(self):
        for _, cb in self.__callbacks:
            cb(self.__input)

class KeyboardInput:
    def __init__(self, name):
        self.name = name
        self.held = False
        self.on_keydown = Connector(self)
        self.on_keyup = Connector(self)

    def keydown(self):
        self.held = True
        self.on_keydown()

    def keyup(self):
        self.held = False
        self.on_keyup()

class MouseInput:
    def __init__(self):
        self.__xrel = self.__yrel = 0

    @property
    def xrel(self):
        ret = self.__xrel
        self.__xrel = 0
        return ret

    @xrel.setter
    def xrel(self, val):
        self.__xrel = val

    @property
    def yrel(self):
        ret = self.__yrel
        self.__yrel = 0
        return ret

    @yrel.setter
    def yrel(self, val):
        self.__yrel = val

class Inputs:
    """Provide signals and state for given bindings.
    """

    def __init__(self, window, bindings):
        self.__map = {}
        for name, key in bindings.items():
            key_input = KeyboardInput(name)
            mod = KeyMod.none
            if isinstance(key, tuple):
                if len(key) != 2:
                    raise Exception(
                        "Cannot bind '{}' with '{}': it should be a tuple "
                        "(KeyMod, key)".format(name, key)
                    )
                key, mod = key

            if isinstance(key, KeySym) or isinstance(key, int):
                pass
            elif isinstance(key, str):
                if len(key) != 1:
                    raise Exception(
                        "Cannot bind '{}' with '{}': it should be a single "
                        "character".format(name, key)
                    )
                key = ord(key)
            else:
                raise Exception(
                    "Cannot bind '{}' with '{}': it should be a single "
                    "character, a KeySym, a key code, or a tuple (KeyMod, key)".format(name, key)
                )
            setattr(self, name, key_input)
            self.__map[(mod, key)] = key_input

        self.mouse = MouseInput()

        self._hdlrs = [
            window.inputs.on_keydown.connect(self.__on_keydown),
            window.inputs.on_keyup.connect(self.__on_keyup),
            window.inputs.on_mousemove.connect(self.__on_mousemove),
        ]

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
