# -*- encoding: utf-8 -*-

import os, platform, sys, sysconfig


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

    default_architecture = platform.architecture()[0][:2] # '32' or '64'

    def __init__(self, name, env, **kwargs):
        self.name = name
        self.upper_name = name.upper()
        self._env = env
        self._params = kwargs

    @property
    def lib_prefix(self):
        fallback = (sys.platform.startswith('win') and [''] or ['lib'])[0]
        print("FALLBACK = '%s'" % fallback, self._params.get('lib_prefix'))
        return self._getprop(
            'lib_prefix',
            global_ = False,
            fallback = (sys.platform.startswith('win') and [''] or ['lib'])[0]
        )

    @property
    def architecture(self):
        return self._getprop(
            'architecture',
            global_ = True,
            fallback = self.default_architecture
        )

    @property
    def prefix(self):
        return self._getprop(
            'prefix',
            global_ = True,
            directory = True,
            hints = ['/usr']
        )

    @property
    def include_dir(self):
        return self._getprop(
            'include_dir',
            global_ = True,
            directory = True,
            hints = [
                cleanjoin(self.prefix, 'include')
            ]
        )


    @property
    def static_library_exts(self):
        if sys.platform.startswith('win'):
            return ['a', 'lib']
        else:
            return ['a']

    @property
    def static_library_dir(self):
        return self._getprop(
            'static_library_dir',
            global_ = True,
            directory = True,
            contains_static_library = True,
            hints = [
                cleanjoin(self.prefix, 'lib'),
                cleanjoin(self.prefix, 'lib' + self.architecture),
                cleanjoin(self.prefix, 'libs'),
            ]
        )

    @property
    def static_library(self):
        try:
            f = self._getprop('static_library_file')
            if f.startswith('lib'):
                return '.'.join(f.split('.')[:-1])[3:]
            else:
                return '.'.join(f.split('.')[:-1])
        except:
            pass

        return self._getprop('static_library', fallback=self.name)


    @property
    def shared_library_exts(self):
        if sys.platform.startswith('win'):
            return ['dll']
        elif sys.platform.startswith('mac'):
            return ['dylib']
        else:
            return ['so']

    @property
    def shared_library_dir(self):
        return self._getprop(
            'shared_library_dir',
            global_ = True,
            directory = True,
            contains_shared_library = True,
            hints = [
                cleanjoin(self.prefix, 'lib'),
                cleanjoin(self.prefix, 'lib' + self.architecture),
                cleanjoin(self.prefix, 'bin'),
                cleanjoin(self.prefix, 'bin' + self.architecture),
                cleanjoin(self.prefix, 'DLLs'),
            ]
        )

    @property
    def shared_library(self):
        try:
            f = self._getprop('shared_library_file')
            if f.startswith('lib'):
                return '.'.join(f.split('.')[:-1])[3:]
            else:
                return '.'.join(f.split('.')[:-1])
        except:
            pass
        return self._getprop('shared_library', fallback=self.name)


    def _getprop(self, prop, global_=None, directory=False, fallback=None,
                 contains_static_library=False,
                 contains_shared_library=False, hints=[]):
        upper_prop_lib = self.upper_name + '_' + prop.upper()
        if global_ is True:
            global_name = prop.upper()
        else:
            global_name = global_
        if prop in self._params:
            res = self._params[prop]
        elif upper_prop_lib in self._env:
            res = self._env[upper_prop_lib]
        elif global_name is not None and global_name in self._env:
            res = self._env[global_name]
        elif fallback is not None:
            res = fallback
        else:
            res = None

        l = res is not None and [res] or []
        l += fallback is not None and [fallback] or []
        l.extend(hints)
        l = list(set(l))
        print("PROP '%s'" % prop, l)

        res = None
        while len(l):
            top = l.pop(0)
            if directory and not os.path.isdir(top):
                print("not dir", top)
                continue
            if contains_static_library and not self._contains_static_library(top):
                print("do not contain static lib", top)
                continue
            if contains_shared_library and not self._contains_shared_library(top):
                print("do not contain shared lib", top)
                continue
            res = top
            break


        if res is None:
            raise Exception(
                "Cannot find %s property for %s \
                (please specify %s or %s with -D switch)." % (
                    prop, self.name, upper_prop_lib, prop.upper()
                )
            )
        if '\\' in res:
            return cleanpath(res)
        return res

    def _contains_static_library(self, dir_, **kwargs):
        kwargs.setdefault('prefixes', set([self.lib_prefix, 'lib']))
        kwargs.setdefault('exts', self.static_library_exts)
        kwargs.setdefault('name', self.name)
        return self._contains(dir_, **kwargs)

    def _contains_shared_library(self, dir_, **kwargs):
        kwargs.setdefault('prefixes', set([self.lib_prefix, 'lib']))
        kwargs.setdefault('exts', self.shared_library_exts)
        kwargs.setdefault('name', self.name)
        return self._contains(dir_, **kwargs)

    def _contains(self, directory, prefixes=[], name=None, exts=[]):
        name = name is None and self.name or name
        for fname in os.listdir(directory):
            for ext in exts:
                for prefix in prefixes:
                    if fname.startswith(prefix + name) and fname.endswith('.' +  ext):
                        return True

        return False


def python_library(globals_):
    prefix = sysconfig.get_config_var('prefix')
    version = sysconfig.get_config_var('VERSION')
    print("PYTHON INCLUDE DIR = ", sysconfig.get_config_vars()['INCLUDEPY'])
    return Library(
        'python',
        env = globals_,
        version = version,
        prefix = prefix,
        include_dir = sysconfig.get_config_vars()['INCLUDEPY'],
        static_library_file = sysconfig.get_config_vars().get('LIBRARY'),
        shared_library_file = sysconfig.get_config_vars().get('LDLIBRARY')
    )


def find_library(name, globals_=globals()):
    if name.lower() + '_library' in globals():
        return globals()[name + '_library'](globals_)
    else:
        return Library(name, globals_)
