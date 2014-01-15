from . import registry

class _AutoRegister(type):

    def __init__(cls, name, bases, dict):
        super().__init__(name, bases, dict)

        case = cls()
        for k, v in dict.items():
            if k.startswith('test'):
                method = getattr(case, k)
                if not callable(method): continue
                registry.add(
                    case.setUp,
                    case.tearDown,
                    method,
                    "unknown",
                    0,
                    k
                )

class Case(metaclass = _AutoRegister):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def assertEqual(self, lhs, rhs):
        if lhs != rhs:
            raise Exception("%s != %s" % (lhs, rhs))

    def assertNotEqual(self, lhs, rhs):
        if lhs == rhs:
            raise Exception("%s == %s" % (lhs, rhs))

    def assertFalse(self, val):
        if val:
            raise Exception("%s evaluates to True" % val)

    def assertTrue(self, val):
        if not val:
            raise Exception("%s evaluates to False" % val)

