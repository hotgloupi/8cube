# -*- encoding: utf8 -*-

from .object import Object

from cube.test import Case

class _(Case):

    def setUp(self):
        class EventManager:
            def register(self, channel, controller):
                self.registered_channel = channel
                self.registered_controller = controller
            def unregister(self, channel, controller):
                self.unregistered_channel = channel
                self.unregistered_controller = controller
        class Game:
            def __init__(self):
                self.event_manager = EventManager()
        self.game = Game()

    def test_init(self):
        o = Object(self.game)

    def test_controller(self):
        from .controller import Controller
        class ControllerTest(Controller):
            channels = ['test', ]
        o = Object(self.game)
        c = o.add_controller(ControllerTest)
        self.assertEqual(
            self.game.event_manager.registered_channel,
            'test'
        )
        self.assertIs(
            self.game.event_manager.registered_controller,
            c
        )
        o.remove_controller(c)
        self.assertEqual(
            self.game.event_manager.unregistered_channel,
            'test'
        )
        self.assertIs(
            self.game.event_manager.unregistered_controller,
            c
        )
