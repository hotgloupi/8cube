# -*- encoding: utf-8 -*-

import os, platform, sys, sysconfig

DEBUG = False
VERBOSE = True

def debug(*msg):
    if DEBUG:
        print('----', *msg)

def log(*msg):
    if VERBOSE:
        print('---', *msg)


def warn(*msg):
    if VERBOSE:
        print('WARNING:', *msg)

def error(*msg):
    print('ERROR:', *msg)


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


    def _library_exts(self, kind, quiet=False):
        if kind == 'static':
            if sys.platform.startswith('win'):
                return ['a', 'lib']
            else:
                return ['a']
        elif kind == 'shared':
            if sys.platform.startswith('win'):
                return ['dll']
            elif sys.platform.startswith('mac'): # XXX darwin
                return ['dylib']
            else:
                return ['so']
        raise Exception("Unknown kind '%s'" % str(kind))

    def _library_dir(self, kind, quiet=False):
        hints = {
            'static': [
                cleanjoin(self.prefix, 'lib'),
                cleanjoin(self.prefix, 'lib' + self.architecture),
                cleanjoin(self.prefix, 'libs'),
            ],
            'shared': [
                cleanjoin(self.prefix, 'lib'),
                cleanjoin(self.prefix, 'lib' + self.architecture),
                cleanjoin(self.prefix, 'bin'),
                cleanjoin(self.prefix, 'bin' + self.architecture),
                cleanjoin(self.prefix, 'DLLs'),
                'c:/Windows/System32',
            ],
        }
        try:
            f = self._getprop(kind + '_library_name', quiet=True)
            match_name = f.startswith('lib') and f[3:] or f
            debug("Using %s_library_name=%s as a fallback name" % (kind, match_name))
        except:
            match_name = self.name

        return self._getprop(
            kind + '_library_dir',
            global_ = True,
            directory = True,
            contains_library = True,
            match_name = match_name,
            kind = kind,
            hints = hints[kind],
            quiet = quiet
        )

    def _library(self, kind, quiet=False):
        assert kind is not None
        try:
            f = self._getprop(kind + '_library_name', quiet=True)
            match_name = f.startswith('lib') and f[3:] or f
            debug("Using %s_library_name=%s as a fallback name" % (kind, match_name))
        except:
            match_name = self.name

        name = self._getprop(kind + '_library', fallback=match_name, quiet=quiet)
        try:
            found = []
            lib_dir = self._library_dir(kind, quiet=quiet)
            if lib_dir is None:
                return name
            for f in os.listdir(self._library_dir(kind)):
                if name in f and any(f.endswith('.' + ext) for ext in self._library_exts(kind)):
                    found.append(f)
            names = []
            for ext in self._library_exts(kind):
                matches = [f for f in found if f.endswith('.' + ext) and not f.startswith('.')]
                for match in matches:
                    debug("Found file '%s'" % match)
                    if match.startswith('lib'):
                        match = match[3:]
                    match = match[:-len(ext) - 1]
                    names.append(match)
                if len(names) == 1:
                    name = names[0]
                    assert name is not None
                    debug("found -l name '%s' for %s %s library" % (name, self.name, kind))
                    return name

            name = min(names)
            assert name is not None

            debug("found -l name '%s' for %s %s library" % (name, self.name, kind))
        except Exception as err:
            from traceback import format_tb
            warn(err, self._library_dir(kind))
            for l in format_tb(err.__traceback__):
                print('##', *(l.split('\n')), sep='\n## ')
        return name



    def _getprop(self, prop, global_=None, directory=False, fallback=None,
                 contains_library=False, match_name=None,
                 kind=None, hints=[],
                 quiet=False):
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

        res = None
        while len(l):
            top = l.pop(0)
            if directory and not os.path.isdir(top):
                continue
            if contains_library and not self._contains_library(top, kind=kind, name=match_name):
                #print("%s does not contain %s library named %s" % (top, kind, match_name))
                continue
            if contains_library and not self._contains_library(top, kind=kind, name=match_name):
                #print("%s does not contain %s library named %s" % (top, kind, match_name))
                continue
            res = top
            break


        if res is None:
            if not quiet:
                error(
                    "Cannot find %s property for %s \
                    (please specify %s or %s with -D switch)." % (
                        prop, self.name, upper_prop_lib, prop.upper()
                    )
                )
        elif '\\' in res:
            return cleanpath(res)
        return res

    def _contains_library(self, dir_, kind=None, **kwargs):
        kwargs.setdefault('prefixes', set([self.lib_prefix, 'lib']))
        kwargs.setdefault('exts', self._library_exts(kind))
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

    @staticmethod
    def gen_property(kind, fname):
        k = '_%s%s_result' % (kind, fname)
        def closure(self):
            if True or not hasattr(self, k):
                log("Search for %s.%s'" % (self.name.upper(), kind + fname))
                func = getattr(self, fname)
                setattr(self, k, func(kind))
            return getattr(self, k)
        return property(closure)

for kind in ['static', 'shared']:
    for fname in ['_library', '_library_exts', '_library_dir']:
        setattr(Library, kind + fname, Library.gen_property(kind, fname))


def python_library(globals_):
    prefix = sysconfig.get_config_var('prefix')
    version = sysconfig.get_config_var('VERSION')
    return Library(
        'python',
        env = globals_,
        version = version,
        prefix = prefix,
        include_dir = sysconfig.get_config_vars()['INCLUDEPY'],
        static_library_file = sysconfig.get_config_vars().get('LIBRARY'),
        shared_library_file = sysconfig.get_config_vars().get('LDLIBRARY')
    )

def GL_library(globals_):
    kwargs = {}
    if sys.platform.startswith('win'):
        kwargs.update({
            'shared_library_name': 'opengl32',
            'shared_library_dir': 'c:\\Windows\\System32',
            'static_library_name': 'opengl32',
            'static_library_dir': 'c:\\Windows\\System32',
        })
    return Library(
        'GL',
        env = globals_,
        **kwargs
    )

def GLEW_library(globals_):
    kwargs = {}
    if sys.platform.startswith('win'):
        kwargs.update({
            'shared_library_name': 'glew32s',
            'static_library_name': 'glew32s',
        })
    return Library(
        'GLEW',
        env = globals_,
        **kwargs
    )

def find_library(name, globals_=globals()):
    if name + '_library' in globals():
        return globals()[name + '_library'](globals_)
    else:
        return Library(name, globals_)
