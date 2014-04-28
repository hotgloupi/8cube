
from cubeapp.game.event import Channel
from cubeapp.game.entity.controller import Controller

from .component import Component

class ChannelError(Exception):
    def __init__(self, channel):
        super().__init__(
            "Wrong channel triggered: %s" % channel
        )

class Controllers(Component):

    name = 'controllers'

    __slots__ = ('controllers', )

    def init(self, controllers = None):
        if controllers is None:
            controllers = []
        else:
            controllers = list(controllers)
        self.controllers = []
        for controller_cls in controllers:
            if not isinstance(controller_cls, type) and issubclass(controller_cls, Controller):
                raise Exception("Expect a controller class, got '%s'" % controller_cls)
            self.add_controller(controller_cls, self.entity)

    def add_controller(self, cls, *args, **kw):
        assert issubclass(cls, Controller)
        controller = self.manager.event_manager.emplace(cls, *args, **kw)
        self.controllers.append(controller)
        return controller

    def remove_controller(self, controller):
        assert controller in self.controllers
        for channel in controller.channels:
            self.manager.event_manager.unregister(controller, channel)
        self.controllers.remove(controller)
        return self

