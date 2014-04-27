
from cubeapp.game.event import Channel

from .component import Component

class ChannelError(Exception):
    def __init__(self, channel):
        super().__init__(
            "Wrong channel triggered: %s" % channel
        )

class Controller(Component):

    name = 'controller'

    __slots__ = ('channels', )

    def init(self, channels = None):
        if channels is None:
            channels = tuple()
        self.channels = tuple(Channel(c) for c in channels)

    def __call__(self, event, elapsed):
        if event.channel not in self.channels:
            raise ChannelError(event.channel)
        self.fire(event, elapsed)

    def fire(self, event, elapsed):
        """Called when an event is triggered.
        """
        pass

