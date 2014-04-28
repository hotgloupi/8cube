# -*- encoding: utf8 -*-
from cubeapp.game.event import Channel

class ChannelError(Exception):
    def __init__(self, channel):
        super().__init__(
            "Wrong channel triggered: %s" % channel
        )

class Controller:

    def __init__(self, entity, *channels):
        assert entity is not None
        self.__entity = entity
        self.__channels = tuple(Channel(c) for c in channels)

    @property
    def entity(self):
        return self.__entity

    @property
    def entity_manager(self):
        return self.__entity.manager

    @property
    def event_manager(self):
        return self.__entity.manager.event_manager

    @property
    def inputs(self):
        return self.__entity.manager.inputs

    @property
    def scene(self):
        return self.__entity.manager.scene

    @property
    def renderer(self):
        return self.__entity.manager.renderer

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

