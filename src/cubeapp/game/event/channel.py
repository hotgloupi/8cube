# -*- encoding: utf8 -*-

import uuid

class Channel:
    """Represent a channel of communication between components.
    """
    def __init__(self, name = None, **kw):
        if name is None:
            name = uuid.uuid1().hex
        elif isinstance(name, Channel):
            name = name.name
        assert isinstance(name, str)
        assert(len(name))
        self.__name = name.lower()
        for k, v in kw.items():
            setattr(self, k, v)

    @property
    def name(self): return self.__name

    def __eq__(self, other):
        if isinstance(other, Channel):
            other = other.name
        assert isinstance(other, str)
        return self.__name == other.lower()

