# -*- encoding: utf8 -*-

from . import Event
from .channel import Channel

class Manager:
    """Store and forward events to registered controllers."""

    def __init__(self):
        self.__queue = []
        self.__controllers = dict()
        self.__tick = Channel('tick')
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

from unittest import TestCase

class TestManager(TestCase):

    def setUp(self):
        self.manager = Manager()
        self.manager.register(Channel("test"), self.__save_event)
        self.evt = None

    def __save_event(self, evt, elapsed_time):
        self.evt = evt

    def test_register(self):
        self.manager.poll(0)
        self.manager.unregister(Channel("test"), self.__save_event)
        self.assertIsNone(self.evt)

    def test_push(self):
        self.manager.poll(0)
        self.assertIsNone(self.evt)
        e = Event("Test")
        self.manager.push(e)
        self.manager.poll(0)
        self.assertIs(self.evt, e)
        self.evt = None
        self.manager.poll(0)
        self.assertIsNone(self.evt)

    def test_unregister(self):
        self.assertIsNone(self.evt)
        self.manager.push(Event("test"))
        self.manager.unregister(Channel("test"), self.__save_event)
        self.assertIsNone(self.evt)
        self.manager.poll(0)
        self.assertIsNone(self.evt)
