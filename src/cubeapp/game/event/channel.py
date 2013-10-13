# -*- encoding: utf8 -*-

import uuid

class Channel:
    """Represent a channel of communication between components.
    """
    def __init__(self, name = None):
        if name is None:
            name = uuid.uuid1().hex
        elif isinstance(name, Channel):
            name = name.name
        assert isinstance(name, str)
        assert(len(name))
        self.__name = name.lower()

    @property
    def name(self): return self.__name

    def __eq__(self, other):
        if isinstance(other, Channel):
            other = other.name
        assert isinstance(other, str)
        return self.__name == other.lower()


from unittest import TestCase, expectedFailure

class TestChannel(TestCase):

    def test_init(self):
        c = Channel('test')
        c = Channel(name = 'test')

    def test_no_name(self):
        self.assertNotEqual(Channel(), Channel())

    def test_not_a_string(self):
        to_test = [
            ['a list', 'of', 'strings'],
            {'a', 'set'},
            {'a': 'dictionary'},
            42,
            '',
        ]
        for e in to_test:
            try:
                Channel(e)
            except:
                pass
            else:
                self.fail("Could create a channel from '%s'" % e)

    same_channel_names = [
        ('same', 'same'),
        ('Case', 'case'),
        ('case', 'Case'),
        ('caSe', 'CAse'),
        ('CASE', 'CAse'),
        ('CASE', 'CASE'),
        ('case', 'CASE'),
    ]

    def test_case_insensitivity(self):
        for rhs, lhs in self.same_channel_names:
            self.assertEqual(Channel(lhs).name, Channel(rhs).name)

    def test_comparable(self):
        for rhs, lhs in self.same_channel_names:
            self.assertEqual(Channel(lhs), Channel(rhs))
        for rhs, lhs in self.same_channel_names:
            self.assertNotEqual(Channel(lhs + "!"), Channel(rhs))
            self.assertNotEqual(Channel(lhs), Channel(rhs + "!"))
            self.assertFalse(Channel(lhs) == Channel(rhs + "!"))
            self.assertFalse(Channel(lhs + "?") == Channel(rhs + "!"))
            self.assertTrue(Channel(lhs + "?") == Channel(rhs + "?"))

    def test_constructible_from_a_channel(self):
        c = Channel('test')
        c2 = Channel(c)
        c3 = Channel(c2)
        c4 = Channel('test')
        self.assertEqual(c, c2)
        self.assertEqual(c, c3)
        self.assertEqual(c, c4)
        self.assertTrue(c is not c2)
        self.assertTrue(c is not c3)
        self.assertTrue(c is not c4)
