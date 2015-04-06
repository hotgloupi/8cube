
from ..system import System

import itertools

import cube

class Component:
    """Base class for every component.
    Components are designed to be aggregated in entities, they should form a
    flat inheritance tree. That is, inherit from another component only if you
    need have a clear gain in terms of code factorization.
    """

    # Unique identifier of the component.
    name = None

    # List of system types that will work on this component.
    systems = []

    def __init__(self, name = None, **kw):
        self.__dict__.update(kw)
        self.name = (name or self.name or self.__class__.__name__).lower()
        for system in self.systems:
            assert isinstance(system, type)
            assert issubclass(system, System)
