# -*- encoding: utf8 -*-

from .controller import Controller

class Object:

    def __init__(self, game):
        self.game = game
        self.__controllers = []

    def add_controller(self, ControllerType, *args, **kw):
        assert issubclass(ControllerType, Controller)
        controller = ControllerType(self, *args, **kw)
        self.__controllers.append(controller)
        for channel in controller.channels:
            self.game.event_manager.register(channel, controller)
        return controller

    def remove_controller(self, controller):
        assert controller in self.__controllers
        for channel in controller.channels:
            self.game.event_manager.unregister(channel, controller)
        self.__controllers.remove(controller)
        return self

