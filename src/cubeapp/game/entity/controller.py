# -*- encoding: utf8 -*-
from .event import Channel

class ChannelError(Exception):
    def __init__(self, channel):
        super().__init__(
            "Wrong channel triggered: %s" % channel
        )

class Controller:

    def __init__(self, target, channels = None):
        assert target is not None
        self.__target = target
        if channels is None:
            channels = tuple()
        self.__channels = tuple(Channel(c) for c in channels)

    @property
    def target(self):
        return self.__target

    @property
    def channels(self):
        return self.__channels

    def __call__(self, event, elapsed):
        if event.channel not in self.channels:
            raise ChannelError(event.channel)
        self.fire(event, elapsed)

    def fire(self, event, elapsed):
        """Called when an event is triggered.
        """
        pass

