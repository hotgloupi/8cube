
from .event import Event
from .channel import Channel

from cube.test import Case

class _(Case):

    def test_init(self):
        e1 = Event("pif")
        e2 = Event("paf")
        c1 = Channel("pouf")
        e3 = Event(c1)
        self.assertIsInstance(e1.channel, Channel)
        self.assertIsInstance(e2.channel, Channel)
        self.assertIsInstance(e3.channel, Channel)

    def test_no_arg(self):
        try:
            Event()
        except:
            pass
        else:
            self.fail("Shouldn't be able to create an event without arguments")

    def test_same_channel(self):
        e1 = Event("pif")
        e2 = Event("pif")
        e3 = Event(Channel("pif"))
        self.assertEqual(e1.channel, e2.channel)
        self.assertEqual(e1.channel, e3.channel)
        self.assertNotEqual(e1, e2)

