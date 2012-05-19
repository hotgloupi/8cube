# -*- encoding: utf-8 -*-

import os, sys, sysconfig

def cleanpath(p):
    p = os.path.normpath(p).replace('\\', '/')
    if p.startswith('./'):
        return p[2:]
    return p

def cleanabspath(p):
    return cleanpath(os.path.abspath(p))

def cleanjoin(*args):
    return cleanpath(os.path.join(*args))

class Library:
    def __init__(self, name, **kwargs):
        self.name = name
        for k, v in kwargs.items():
            setattr(self, k, v)

def python_library(globals_):
    prefix = sysconfig.get_config_var('prefix')
    version = sysconfig.get_config_var('VERSION')
    return Library(
        'python',
        version = version,
        prefix = prefix,
        include_dir = cleanjoin(prefix, 'include'),
        static_library_dir = cleanjoin(prefix, 'libs'),
        static_library = 'python' + version.replace('.', '')
    )

def glew_library(globals_):
    prefix = globals_.get(
        'GLEW_PREFIX',
        globals_.get('PREFIX')
    )
    if prefix is None:
        raise Exception("Cannot find glew (set PREFIX or GLEW_PREFIX)")
    version = ''

    return Library(
        'glew',
        version = version,
        prefix = prefix,
        include_dir = cleanjoin(prefix, 'include'),
        static_library_dir = cleanjoin(prefix, 'lib'),
        shared_library_dir = cleanjoin(prefix, 'bin'),
        static_library = 'glew32',
        shared_library = 'glew32'
    )


_hooks = {
    'python': python_library,
}

def find_library(name, globals_=globals()):
    return globals()[name.lower() + '_library'](globals_)
