# -*- encoding: utf-8 -*-

class EventResponder:

    def __init__(self):
        self.__next_id = 0
        self.__callbacks = {}

    def connect(self, cb):
        id = self.__next_id
        self.__next_id += 1
        self.__callbacks[id] = cb
        return id

    def disconnect(self, id):
        self.__callbacks.pop(id)

    def __call__(self, ev):
        for cb in self.__callbacks.values():
            cb(ev)

class Responder:
    """Responsible of handling and forward events.
    """

    connectors = [
        'mousemove',
        'mouseenter',
        'mouseleave',
        'mouseclick',

        'keydown',
        'keyup',
    ]

    def __init__(self):
        for s in self.connectors:
            setattr(self, s, EventResponder)

    def fire_event(self, ev):
        pass
