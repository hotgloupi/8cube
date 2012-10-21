# -*- encoding: utf-8 -*-

from cube.system.inputs import KeyMod, KeySym

class KeyboardInput:
    def __init__(self, name):
        self.name = name

    def keydown(self):
        print("KEYDOWN", self.name)


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



        window.inputs.on_keydown.connect(self.__on_keydown)

    def __on_keydown(self, keymod, keysym, keycode):
        keymod = KeyMod(keymod & ~KeyMod.num)
        i = self.__map.get((keymod, keysym))
        if i is None:
            i = self.__map.get((keymod, keycode))
        if i is not None:
            i.keydown()

        print("PIF", KeyMod(keymod), keysym, chr(keycode))

