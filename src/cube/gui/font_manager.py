# -*- encoding: utf-8 -*-

import os
import threading

from cube.gl import font

class FontManager:
    __mutex = threading.Lock()

    __font_instances = {}

    def __init__(self, renderer):
        self.renderer = renderer

    def search(self, name, size):
        results = []
        for item in self._get_fonts().values():
            lock, infos = item
            if lock.locked():
                with lock:
                    _, infos = item
            if infos is None:
                continue
            if self._match(name, infos):
                results.append(self._font_instance(infos, size))
        return results

    @staticmethod
    def _match(name, infos):
        return name.lower() in infos.family_name.lower()

    def _font_instance(self, infos, size):
        instance = self.__font_instances.get(infos.path)
        if instance is None:
            instance = (infos, font.Font(self.renderer, infos.path, size))
            self.__font_instances[infos.path] = instance
        return instance

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
    if font.can_load_file(path):
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
        FontManager._unlock(True)
        return
    else:
        FontManager._unlock(False)

    import time
    start = time.time()
    for path, infos in fonts.items():
        try:
            infos[1] = font.get_infos(path)
        except:
            print("Ignoring font", path)
        finally:
            infos[0].release()
    print("all font infos fetched in %f seconds" % (time.time() - start))


threading.Thread(target=__populate_fonts).start()

