# -*- encoding: utf-8 -*-

import os
import threading

from cube.gl import font

class FontManager:
    __mutex = threading.Lock()

    def __init__(self):
        raise Exception("Cannot instanciate a font manager")

    @classmethod
    def search(cls,
               family=None,
               scalable=None,
               fixed_width=None):
        results = []
        for path, item in cls._get_fonts().items():
            lock, infos_list = item
            if lock.locked():
                infos_list = font.get_infos(path)
                print("fetch locked font", family)
            if infos_list is None:
                continue
            results.extend(
                cls._find_matches(
                    infos_list,
                    family=family,
                    scalable=scalable,
                    fixed_width=fixed_width,
                )
            )
        return results

    @staticmethod
    def _find_matches(infos_list,
                      family=None,
                      scalable=None,
                      fixed_width=None):
        matching_infos = []
        for infos in infos_list:
            if family is not None and family.lower() not in infos.family_name.lower():
                continue
            if scalable is not None and infos.is_scalable != scalable:
                continue
            if fixed_width is not None and infos.is_fixed_width != fixed_width:
                continue
            matching_infos.append(infos)
        return matching_infos

    @classmethod
    def _get_fonts(cls):
        return cls._get_fonts_error()

    @classmethod
    def _lock(cls):
        assert not cls.__mutex.locked()
        cls.__mutex.acquire()
        cls._get_fonts = cls._get_fonts_locked

    @classmethod
    def _unlock(cls, had_error):
        assert cls.__mutex.locked()
        if had_error:
            cls._get_fonts = cls._get_fonts_error
        else:
            cls._get_fonts = cls._get_fonts_ready
        cls.__mutex.release()

    @classmethod
    def _get_fonts_locked(cls):
        print("get locked fonts")
        with cls.__mutex:
            return cls._get_fonts_ready()

    @classmethod
    def _get_fonts_ready(cls):
        return cls._fonts

    @classmethod
    def _get_fonts_error(cls):
        raise Exception("Not initialized")

font_directories = list(
    os.path.expanduser(p) for p in [
        '~/Library/Fonts',
        '/Library/Fonts',
        '/System/Library/Fonts',
        '/usr/share/fonts',
        '/usr/local/share/fonts',
        '~/.fonts',
        '/opt/share/fonts',
        '/opt/local/share/fonts',
] if os.path.isdir(os.path.expanduser(p)))

def __add_font_file(path, fonts):
    if font.is_valid(path):
        lock = threading.Lock()
        fonts[path] = [lock, None]
        lock.acquire()


def __populate_fonts():
    FontManager._lock()
    try:
        fonts = {}
        import time
        start = time.time()
        for font_dir in font_directories:
            for root, dirs, files in os.walk(font_dir):
                for f in files:
                    __add_font_file(os.path.join(root, f), fonts)
        FontManager._fonts = fonts
    except:
        print("WARNING: got exception while initializing the FontManager")
        FontManager._unlock(True)
        return
    else:
        FontManager._unlock(False)

    import time
    start = time.time()
    for path, infos in fonts.items():
        try:
            infos[1] = font.get_infos(path)
        except Exception as e:
            print("Ignoring font", path, e)
        finally:
            infos[0].release()
    print(len(fonts), "font infos fetched in %f seconds" % (time.time() - start))


threading.Thread(target=__populate_fonts).start()

