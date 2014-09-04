from . import registry

import gc
import inspect
import os

_registered = set()
class _AutoRegister(type):

    root_dir = os.path.abspath(os.path.join(__file__, '../../..'))

    def __init__(cls, name, bases, dict):
        super().__init__(name, bases, dict)

        case = cls()
        for k, v in dict.items():
            if k.startswith('test'):
                method = getattr(case, k)
                if not callable(method): continue

                while hasattr(v, '__wrapped__'):
                    v = v.__wrapped__
                if v in _registered:
                    continue
                _registered.add(v)

                file, line = (
                    os.path.relpath(inspect.getfile(cls), start = cls.root_dir),
                    inspect.getsourcelines(v)[1],
                )
                test_name = name + '.' + k
                registry.add(
                    case._Case__setUp,
                    case._Case__tearDown,
                    method,
                    file,
                    line,
                    test_name
                )

class Case(metaclass = _AutoRegister):

    def __setUp(self):
        self.setUp()

    def __tearDown(self):
        self.tearDown()
        gc.collect()

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def assertEqual(self, lhs, rhs, msg = None):
        if lhs != rhs:
            if msg is None:
                msg = "%s != %s" % (lhs, rhs)
            raise Exception(msg)

    def assertNotEqual(self, lhs, rhs):
        if lhs == rhs:
            raise Exception("%s == %s" % (lhs, rhs))

    def assertAlmostEqual(self, lhs, rhs, delta, msg = None):
        if abs(lhs - rhs) > delta:
            raise Exception(
                "%s is not equal to %s: %s greater than allowed delta %s" % (
                    lhs, rhs, abs(lhs - rhs), delta
                ) + (msg and (": " + msg) or ""))

    def assertFalse(self, val):
        if val:
            raise Exception("%s evaluates to True" % val)

    def assertTrue(self, val, msg = None):
        if not val:
            if msg is None:
                msg = "%s evaluates to False" % val
            raise Exception("%s evaluates to False" % val)

    def assertIsNone(self, val):
        if val is not None:
            raise Exception("%s is not None" % val)
    def assertIsNotNone(self, val):
        if val is None:
            raise Exception("value is None")

    def assertIs(self, lhs, rhs):
        if lhs is not rhs:
            raise Exception("%s is not %s" % (lhs, rhs))

    def assertIn(self, lhs, rhs):
        if lhs not in rhs:
            raise Exception("%s not in %s" % (lhs, rhs))

    def assertNotIn(self, lhs, rhs):
        if lhs in rhs:
            raise Exception("%s in %s" % (lhs, rhs))

    def assertIsInstance(self, val, types):
        if not isinstance(val, types):
            raise Exception("%s is not an instance of types %s" % (val, types))

    def assertGreater(self, lhs, rhs):
        if lhs <= rhs:
            raise Exception("%s is less than %s" % (lhs, rhs))

    def assertLessEqual(self, lhs, rhs):
        if lhs > rhs:
            raise Exception("%s is greater than %s" % (lhs, rhs))
    def fail(self, msg):
        raise Exception(msg)
