
from .channel import Channel

class Event:
    """Event base class.

    Each event belong to a 'channel', and will be distributed to every
    subscriber of that channel.
    @note Event names (or channels) are lower cased
    """
    def __init__(self, name):
        self.__channel = Channel(name)

    @property
    def channel(self):
        return self.__channel
