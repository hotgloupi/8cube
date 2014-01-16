#-*- encoding: utf8 -*-

from . import inputs

from cube.test import Case
import functools

class _(Case):
    slots = [
        'on_expose',
        'on_idle',
        'on_keydown',
        'on_keyup',
        'on_mousemove',
        'on_quit',
        'on_resize',
    ]

    def setUp(self):
        self.inputs = inputs.Inputs()
        for attr in self.slots:
            connection = getattr(self.inputs, attr).connect(
                functools.partial(self._on_event, attr)
            )
            setattr(self, attr + '_connection', connection)
        self.on_event = None

    def tearDown(self):
        for attr in self.slots:
            getattr(self, attr + '_connection').disconnect()

    def _on_event(self, *args):
        if self.on_event is not None:
            self.on_event(*args)

    def test_setup(self):
        def store_event(*args): self.args = args
        self.on_event = store_event
        self.inputs.on_expose(12, 12)
        self.assertEqual(self.args, ('on_expose', 12, 12))
        self.inputs.on_expose(42, 42)
        self.assertEqual(self.args, ('on_expose', 42, 42))
        self.on_expose_connection.disconnect()
        self.inputs.on_expose(100, 100)
        self.assertEqual(self.args, ('on_expose', 42, 42))
