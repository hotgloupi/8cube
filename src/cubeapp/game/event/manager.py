# -*- encoding: utf8 -*-

from .event import Event
from .channel import Channel

class Manager:
    """Store and forward events to registered controllers."""

    def __init__(self, tick_channel = Channel('tick')):
        self.__queue = []
        self.__controllers = dict()
        self.__tick = tick_channel
        self.__tick_event = Event(self.__tick)

    def push(self, event):
        assert event.channel != self.__tick
        self.__queue.append(event)

    def register(self, channel, controller):
        assert isinstance(channel, Channel)
        list = self.__controllers.setdefault(channel.name, [])
        assert controller not in list
        list.append(controller)

    def unregister(self, channel, controller):
        assert isinstance(channel, Channel)
        list =  self.__controllers[channel.name]
        assert controller in list
        list.remove(controller)

    def poll(self, time_elapsed):
        queue = self.__queue[:]
        self.__queue = []
        for event in queue:
            for controller in self.__controllers.get(event.channel.name, []):
                controller(event, time_elapsed)
        for controller in self.__controllers.get(self.__tick.name, []):
            controller(self.__tick_event, time_elapsed)

