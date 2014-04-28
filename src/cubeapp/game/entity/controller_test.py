# -*- encoding: utf8 -*-

from .event import Event
from .controller import Controller, ChannelError

from cube.test import Case

class _(Case):

    def test_target(self):
        class Target: pass
        target = Target()
        c = Controller(target)
        self.assertIs(c.target, target)

    def test_operator_call(self):
        class ControllerTest(Controller):
            channels = ["test",]
        class Target: pass
        c = ControllerTest(Target())
        c(Event("test"), 0)

    def test_wrong_channel(self):
        class ControllerTest(Controller):
            channels = ["test",]
        class Target: pass
        c = ControllerTest(Target())
        try:
            c(Event("test_wrong"), 0)
        except ChannelError:
            pass # expected exception
        else:
            this.fail("Should have thrown an exception of type ChannelError")

