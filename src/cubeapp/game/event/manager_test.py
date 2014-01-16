# -*- encoding: utf8 -*-

from .channel import Channel
from .manager import Manager
from . import Event

from cube.test import Case

class _(Case):

    def setUp(self):
        self.manager = Manager()
        self.manager.register(Channel("test"), self.__save_event)
        self.evt = None
        self.elapsed_time = None

    def __save_event(self, evt, elapsed_time):
        self.evt = evt
        self.elapsed_time = elapsed_time

    def test_register(self):
        self.manager.poll(0)
        self.manager.unregister(Channel("test"), self.__save_event)
        self.assertIsNone(self.evt)
        self.assertIsNone(self.elapsed_time)

    def test_push(self):
        self.manager.poll(0)
        self.assertIsNone(self.evt)
        e = Event("Test")
        self.manager.push(e)
        self.manager.poll(12)
        self.assertIs(self.evt, e)
        self.assertEqual(self.elapsed_time, 12)
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
