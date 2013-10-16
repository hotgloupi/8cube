# -*- encoding: utf8 -*-

class ChannelError(Exception):
    def __init__(self, channel):
        super().__init__(
            "Wrong channel triggered: %s" % channel
        )

class Controller:
    channels = []

    def __init__(self, target):
        assert target is not None
        self.__target = target

    @property
    def target(self):
        return self.__target

    def __call__(self, event, elapsed):
        if event.channel not in self.channels:
            raise ChannelError(event.channel)
        self.fire(event, elapsed)

    def fire(self, event, elapsed):
        """Called when an event is triggered.
        """
        pass

