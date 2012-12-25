# -*- encoding: utf-8 -*-

import os
import time

try:
    import _pickle as pickle
except:
    print("WARNING: optimized pickle not available")
    import pickle


from cube.gl import font

class FontManager:

    fonts = None

    def __init__(self):
        raise Exception("Cannot instanciate a font manager")

    @classmethod
    def search(cls,
               family=None,
               scalable=None,
               fixed_width=None):
        results = []
        for path, infos_list in cls.fonts.items():
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
    def populate(cls):
        if cls.fonts is not None:
            return

        start = time.time()
        from cube.constants.application import config_directory
        fonts_dir = os.path.join(config_directory(), "fonts")
        if not os.path.exists(fonts_dir):
            os.makedirs(fonts_dir)
        fonts_file = os.path.join(fonts_dir, "fonts.lst")
        if os.path.exists(fonts_file):
            print("Loading fonts from cache file '%s'" % fonts_file)
            try:
                with open(fonts_file, 'rb') as f:
                    cls.fonts = pickle.loads(f.read())
            except Exception as e:
                print("WARNING: font cache file '%s' is not valid, it will be removed:" % fonts_file, e)
                os.unlink(fonts_file)
        else:
            print("Finding fonts on your system, this may take a while...")
            cls.fonts = {}
            for font_dir in cls.font_directories():
                for root, dirs, files in os.walk(font_dir):
                    for f in files:
                        path = os.path.join(root, f)
                        if font.is_valid(path):
                            cls.fonts[path] = font.get_infos(path)

        print(len(cls.fonts), "font infos fetched in %f seconds" % (time.time() - start))

        print("Saving fonts into cache file '%s'" % fonts_file)
        with open(fonts_file, 'wb') as f:
            f.write(pickle.dumps(cls.fonts))

    @classmethod
    def font_directories(cls):
        return list(
            os.path.expanduser(p) for p in [
                '~/Library/Fonts',
                '/Library/Fonts',
                '/System/Library/Fonts',
                '/usr/share/fonts',
                '/usr/local/share/fonts',
                '~/.fonts',
                '/opt/share/fonts',
                '/opt/local/share/fonts',
            ] if os.path.isdir(os.path.expanduser(p))
        )




