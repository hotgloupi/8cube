# -*- encoding: utf-8 -*-

import os, platform, sys, sysconfig

DEBUG = False
VERBOSE = False

def debug(*msg):
    if DEBUG:
        print('--------[', *msg)

def log(*msg):
    if VERBOSE:
        print('***', *msg)


def warn(*msg):
    print('WARNING:', *msg)

def error(*msg):
    print('# ERROR:', *msg)


def cleanpath(p):
    p = os.path.normpath(p).replace('\\', '/')
    if p.startswith('./'):
        return p[2:]
    return p

def cleanabspath(p):
    return cleanpath(os.path.abspath(p))

def cleanjoin(*args):
    return cleanpath(os.path.join(*args))

def remove_ext(f):
    return split_ext(f)[0]

def get_ext(f):
    return split_ext(f)[1]

def split_ext(f):
    if '.' in f and f.index('.') > 0:
        if '.so.' in f:
            i = f.index('.so')
            return (f[:i], f[i + 1:])
        parts = f.split('.')
        return (parts[0], '.'.join(parts[1:]))
    else:
        return (f, '')

def cached(func):
    _ = {}
    def wrapper(*args, **kwargs):
        k = args + tuple(kwargs.items())
        if k in _:
            return _[k]
        _[k] = v = func(*args, **kwargs)
        return v
    wrapper.__name__ == func.__name__
    return wrapper


class Library:

    default_architecture = platform.architecture()[0][:2] # '32' or '64'

    def __init__(self, name, env, **kwargs):
        self.name = name
        self.upper_name = name.upper()
        self._env = env
        self._params = kwargs


    @property
    @cached
    def lib_prefix(self):
        fallback = (sys.platform.startswith('win') and [''] or ['lib'])[0]
        return self._getprop(
            'lib_prefix',
            global_ = False,
            fallback = (sys.platform.startswith('win') and [''] or ['lib'])[0]
        )

    @property
    @cached
    def architecture(self):
        return self._getprop(
            'architecture',
            global_ = True,
            fallback = self.default_architecture
        )

    @property
    @cached
    def prefix(self):
        return self._getprop(
            'prefix',
            global_ = True,
            directory = True,
            hints = ['/usr']
        )

    @property
    @cached
    def include_dir(self):
        return self._getprop(
            'include_dir',
            global_ = True,
            directory = True,
            hints = [
                cleanjoin(self.prefix, 'include')
            ]
        )


    @cached
    def _library_exts(self, kind, quiet=False):
        if kind == 'static':
            if sys.platform.startswith('win'):
                return ['dll.a', 'a', 'lib',]
            else:
                return ['a']
        elif kind == 'shared':
            if sys.platform.startswith('win'):
                return ['dll', 'lib']
            elif sys.platform.startswith('mac'): # XXX darwin
                return ['dylib']
            else:
                return ['so']
        raise Exception("Unknown kind '%s'" % str(kind))

    @cached
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

    @cached
    def _is_library_file(self, kind, dir_, f, allow_symlink=False):
        fext = get_ext(f)
        valid_ext = any(
            (fext == ext) for ext in self._library_exts(kind)
        )
        if not valid_ext:
            return False

        path = os.path.join(dir_, f)
        res = (
            (
                allow_symlink or not os.path.islink(path)
            ) and (
                kind == 'static' or os.access(path, os.X_OK)
            )
        )
        return res

    @cached
    def _library_filename(self, kind, quiet=False, allow_symlink=False, shortest=True):
        assert kind is not None
        try:
            f = self._getprop(kind + '_library_name', quiet=True)
            debug("Found prop:", f)
            match_name = f.startswith('lib') and f[3:] or f
            debug("Using %s_library_name=%s as a fallback name" % (kind, match_name))
        except:
            match_name = self.name

        name = self._getprop(kind + '_library', fallback=match_name, quiet=quiet)
        if name.startswith('lib'):
            name = name[3:]
        match_name = name
        debug("Searching for library filename of", match_name, name)
        result = None
        try:
            found = []
            lib_dir = self._library_dir(kind, quiet=quiet)
            if lib_dir is None:
                warn("default name picked because the library directory of", self.name, "coudn't be found")
                return name + '.' + self._library_exts(kind, quiet=quiet)[0]
            for f in os.listdir(lib_dir):
                if name in f and not f.startswith('.'):
                    if self._is_library_file(kind, lib_dir, f, allow_symlink = allow_symlink):
                        found.append(f)
                    else:
                        debug("Ignore file", f)

            def match_ref(ref, n):
                if ref == n:
                    return True
                if n.startswith('lib'):
                    n = n[3:]
                if ref == n:
                    return True
                n = n.strip('.123456789').lower()
                if ref == n:
                    return True
                if '-' in n:
                    n = n.split('-')[0]
                return n == ref
            files = []
            exts = self._library_exts(kind)
            for f in found:
                fname, fext = split_ext(f)
                if not match_ref(match_name, fname):
                    debug("Ignore result", f, '(', fname, 'does not match', match_name, ')')
                    continue
                for ext in exts:
                    if fext.startswith(ext):
                        debug("Found file '%s'" % f)
                        files.append(f)
                        break
                #if len(files) == 1: #first extension might be the most accurate
                #    result = files[0]
                #    if self._is_library_file(kind, lib_dir, result, allow_symlink = False):
                #        assert result is not None
                #        return result

            if not files:
                raise Exception("Cannot find library name for " + name)
            if shortest:
                result = sorted(files, key=lambda k: len(k))[0]
            else:
                result = sorted(files, key=lambda k: -len(k))[0]
            debug("Choose %s in ("  % match_name, files, "):", result)
            assert result is not None

        except Exception as err:
            from traceback import format_tb
            warn(err, self._library_dir(kind))
            if VERBOSE:
                for l in format_tb(err.__traceback__):
                    print('##', *(l.split('\n')), sep='\n## ')
        return result

    @cached
    def _library_filepath(self, kind, quiet=False):
        d = self._library_dir(kind, quiet=quiet)
        fname = self._library_filename(kind, quiet=quiet)
        if d is None or fname is None:
            return None
        return cleanjoin(d, fname)

    @cached
    def _library(self, kind, quiet=False):
        fname = self._library_filename(kind, quiet=quiet, allow_symlink=True, shortest=True)
        print("FOUND", fname)
        if fname is None:
            return None
        if fname.startswith('lib'):
            fname = fname[3:]
        name, ext = split_ext(fname)
        if '.' in ext and not ext.startswith('so.'):
            name += '.' + '.'.join(ext.split('.')[:-1]) #+ '-PATCHED-'+kind+'-' + name + '-' + ext
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
        unique = set(l)
        l = list(e for e in l if e in unique and len([unique.remove(e)]))
        debug("Searching property '%s'" % prop, prop, "in", l)

        res = None
        while len(l):
            top = l.pop(0)
            if directory and not os.path.isdir(top):
                continue
            if contains_library and not self._contains_library(top, kind=kind, name=match_name):
                debug("%s does not contain %s library named %s" % (top, kind, match_name))
                continue
            res = top
            break


        if res is None:
            if not quiet:
                warn("Cannot find %s property for %s." % (prop, self.name))
                warn("Please define %s or %s (with -D)" % (upper_prop_lib, prop.upper()))
        elif '\\' in res:
            return cleanpath(res)
        return res

    def _contains_library(self, dir_, kind=None, **kwargs):
        kwargs.setdefault('prefixes', tuple(set([self.lib_prefix, 'lib'])))
        kwargs.setdefault('exts', self._library_exts(kind))
        kwargs.setdefault('name', self.name)
        return self._contains(dir_, **kwargs)

    def _contains(self, directory, prefixes=(), name=None, exts=()):
        name = name is None and self.name or name
        debug("searching", name, "in", directory, '(with exts =', exts,')')
        for fname in os.listdir(directory):
            for ext in exts:
                for prefix in prefixes:
                    n, e = split_ext(fname)
                    if e == ext and fname.startswith(prefix + name):
                        debug("found match for file", fname, '(', n, e,')')
                        return True

        return False

    @staticmethod
    def gen_property(kind, fname):
        def closure(self):
            func = getattr(self, fname)
            log("Search for %s.%s'" % (self.name.upper(), kind + fname))
            return func(kind)
        return property(closure)

for kind in ['static', 'shared']:
    for fname in ['_library', '_library_exts', '_library_dir', '_library_filename', '_library_filepath']:
        setattr(Library, kind + fname, Library.gen_property(kind, fname))


def python_library(globals_):
    cfg = sysconfig.get_config_vars()
    prefix = cfg['prefix']
    version = cfg['VERSION']
    kwargs = {
        'env': globals_,
        'version': version,
        'prefix': prefix,
        'include_dir': cfg['INCLUDEPY'],
        'static_library_name': remove_ext(cfg.get('LIBRARY', 'python' + version.replace('.', ''))),
        'shared_library_name': remove_ext(cfg.get('LDLIBRARY', 'python' + version.replace('.', ''))),
    }
    return Library('python', **kwargs)

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
