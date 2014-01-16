# -*- encoding: utf-8 -*-

from . import exception

from cube.test import Case

class _(Case):

    def test_raise_from_cpp(self):
        try:
            exception.test_throw("TestException")
        except exception.Exception as e:
            self.assertEqual(str(e), "TestException")
        else:
            self.fail("Coudn't catch the right exception type")

    def test_raise_from_python(self):
        try:
            raise exception.Exception("TestException")
        except exception.Exception as e:
            self.assertEqual(str(e), "TestException")
        else:
            self.fail("Couldn't catch the right exception type")

    def test_raise_from_cpp_in_a_python_callback(self):
        def cb():
            raise exception.Exception("TestException")
        try:
            exception.test_cb(cb)
        except exception.Exception as e:
            self.assertEqual(str(e), "TestException")
        else:
            self.fail("Couldn't catch the right exception type")
