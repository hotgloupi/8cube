
from .channel import Channel

class Event:
    """Event base class.

    Each event belong to a 'channel', and will be distributed to every
    subscriber of that channel.
    @note Event names (or channels) are lower cased
    """
    def __init__(self, channel, **kw):
        if not isinstance(channel, Channel):
            channel = Channel(channel)
        self.__channel = channel
        for k, v in kw.items():
            setattr(self, k, v)

    @property
    def channel(self):
        return self.__channel

    def __str__(self):
        return '<Event of %s>' % self.__channel.name
