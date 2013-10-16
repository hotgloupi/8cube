# -*- encoding: utf8 -*-

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
        hdlr = self.Handler(self)
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
        self.key_pressed = Connector(self)
        self.key_released = Connector(self)
        self.key_held = Connector(self)

