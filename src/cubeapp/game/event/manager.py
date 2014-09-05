# -*- encoding: utf8 -*-

from .event import Event
from .channel import Channel

class Manager:
    """Store and forward events to registered controllers."""

    def __init__(self, tick_channel = Channel('tick')):
        self.__tick = tick_channel
        self.__tick_event = Event(self.__tick)
        self.__delay_channel = Channel()
        self.clear()

    def clear(self):
        self.__cleared = True
        self.__queue = []
        self.__delayed_queue = []
        self.__controllers = dict()
        self.register(lambda ev, elapsed: ev.cb(elapsed), self.__delay_channel)

    def call_later(self, cb, delay):
        self.__delayed_queue.append(
            (delay, Event(self.__delay_channel, cb = cb))
        )

    def push(self, event, delay = None):
        assert event.channel != self.__tick
        if delay:
            self.__delayed_queue.append((time.time() + delay, event))
        else:
            self.__queue.append(event)

    def emplace(self, controller_class, *args, **kw):
        """Construct and add a controller the given class."""
        return self.add(controller_class(*args, **kw))

    def add(self, controller):
        """register controller to each channels in controller.channels.
        """
        for ch in controller.channels:
            self.register(controller, ch)
        return controller

    def register(self, controller, channel):
        if isinstance(channel, str):
            channel = Channel(channel)
        assert isinstance(channel, Channel)
        list = self.__controllers.setdefault(channel.name, [])
        assert controller not in list
        list.append(controller)

    def unregister(self, controller, channel = None):
        if channel is not None:
            channels = [channel]
        else:
            channels = controller.channels
        del channel
        for ch in channels:
            assert isinstance(ch, Channel)
            l = self.__controllers[ch.name]
            assert controller in l
            l.remove(controller)


    def poll(self, time_elapsed):
        self.__cleared = False
        queue = self.__queue[:]
        self.__queue = []
        delayed_queue = self.__delayed_queue[:]
        self.__delayed_queue = []
        for ts, event in delayed_queue:
            ts -= time_elapsed
            if ts < 0:
                queue.append(event)
            else:
                self.__delayed_queue.append((ts, event))

        for event in queue:
            for controller in self.__controllers.get(event.channel.name, []):
                controller(event, time_elapsed)
                if self.__cleared:
                    return


        for controller in self.__controllers.get(self.__tick.name, []):
            controller(self.__tick_event, time_elapsed)

