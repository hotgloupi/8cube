# -*- encoding: utf8 -*-

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
    def channel(self): return self.__channel


from unittest import TestCase, expectedFailure

class TestEvent(TestCase):

    def test_init(self):
        e1 = Event("pif")
        e2 = Event("paf")
        c1 = Channel("pouf")
        e3 = Event(c1)
        self.assertIsInstance(e1.channel, Channel)
        self.assertIsInstance(e2.channel, Channel)
        self.assertIsInstance(e3.channel, Channel)

    @expectedFailure
    def test_no_arg(self):
        Event()

    def test_same_channel(self):
        e1 = Event("pif")
        e2 = Event("pif")
        e3 = Event(Channel("pif"))
        self.assertEqual(e1.channel, e2.channel)
        self.assertEqual(e1.channel, e3.channel)
        self.assertNotEqual(e1, e2)

