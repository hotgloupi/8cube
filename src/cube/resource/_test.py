from . import Resource, Manager

from unittest import TestCase
import gc

class Core(TestCase):

    def setUp(self):
        self.manager = Manager()

    def tearDown(self):
        self.manager = None
        gc.collect()

    def test_init(self):
        r = Resource()
        self.assertFalse(r.managed)
        gc.collect()
        self.assertEqual(len(gc.get_referrers(r)), 1)
        self.assertEqual(r.refcount, 1)

    def test_managed_resource(self):
        r = Resource()
        self.manager.manage(r)
        self.assertTrue(r.managed)
        gc.collect()
        self.assertEqual(len(gc.get_referrers(r)), 1)
        self.assertEqual(r.refcount, 2)
        self.manager.forget(r)
        self.assertFalse(r.managed)
        self.assertEqual(r.refcount, 1)

    def test_custom_resource(self):
        class MyResource(Resource):
            def __init__(self, name, other = None):
                super().__init__()
                self.name = name
                self.other = other
        ctor_args = [
            (("Pif", "paf"), {}),
            (("Pif",), {'other': "paf"}),
            ((), {'name':"Pif", 'other': "paf"})
        ]
        for args, kw in ctor_args:
            r = MyResource(*args, **kw)
            self.assertFalse(r.managed)
            self.assertEqual(r.refcount, 1)
            self.assertEqual(r.name, "Pif")
            self.assertEqual(r.other, "paf")
            gc.collect()
            self.assertEqual(len(gc.get_referrers(r)), 1)
            self.assertEqual(r.refcount, 1)

    def test_create_resource(self):
        r = self.manager.create(Resource)
        gc.collect()
        self.assertEqual(len(gc.get_referrers(r)), 1)
        self.assertEqual(r.refcount, 2)
        self.manager.forget(r)
        self.assertEqual(r.refcount, 1)

    def test_delete_manager_early(self):
        r = self.manager.create(Resource)
        self.assertEqual(r.refcount, 2)
        self.manager = None
        gc.collect()
        self.assertEqual(r.refcount, 1)
        self.assertFalse(r.managed)

    def test_create_custom_resource(self):
        class MyResource(Resource):
            def __init__(self, name, other = None):
                super().__init__()
                self.name = name
                self.other = other
        ctor_args = [
            (("pif", "lol"), {}),
            (("pif",), {'other': "lol"}),
            ((), {'name': "pif", 'other': "lol"})
        ]
        for args, kw in ctor_args:
            r = self.manager.create(MyResource, *args, **kw)
            self.assertEqual(r.refcount, 2)
            self.assertTrue(r.managed)
            self.assertEqual(r.name, "pif")
            self.assertEqual(r.other, "lol")
            self.manager.forget(r)
            gc.collect()
            self.assertEqual(r.refcount, 1)

    def test_create_from_not_a_type(self):
        for value in [42, "lol", self, set(), tuple(), lambda:42]:
            try:
                self.manager.create(12)
            except ValueError as e:
                self.assertEqual(str(e), "First argument should be a type")
            else:
                self.fail("Creating a resource from anything but a type should "
                          "throw a ValueError")

    def test_create_from_not_a_resource_type(self):
        class Pif:
            pass

        for type in [Pif, int, str, float, Manager]:
            try:
                self.manager.create(Pif)
            except TypeError as e:
                self.assertEqual(
                    str(e),
                    "The resource type must be a subclass of Resource"
                )
            else:
                self.fail("Creating a resource from a type that is not a "
                          "subclass of Resource should throw a TypeError")


    def test_create_custom_resource_forgotten_ctor_super(self):
        class MyResource(Resource):
            def __init__(self):
                pass
        try:
            r = self.manager.create(MyResource)
        except TypeError as e:
            self.assertTrue(
                str(e).startswith(
                    "Couldn't cast the created resource to the Resource type"
                )
            )
        else:
            self.fail("Calling the base constructor is required")

    def test_create_custom_resource_that_throws(self):
        class MyException(Exception):
            pass
        class MyResource(Resource):
            def __init__(self):
                raise MyException("MyResource always throws")

        try:
            r = self.manager.create(MyResource)
        except MyException as e:
            self.assertEqual(str(e), "MyResource always throws")
        else:
            self.fail("An exception thrown in a Resource constructor should be "
                      " left intact")


