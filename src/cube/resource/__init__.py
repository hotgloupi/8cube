#-*- encoding: utf8 -*-

from ._resource import Resource, _Manager

class Manager(_Manager):

    def create(self, type, *args, **kw):
        return super().create(type, args, kw)
