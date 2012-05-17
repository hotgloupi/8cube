#!/usr/bin/env python
# -*- encoding: utf-8 -*-

import sys
import shutil

if __name__ == '__main__':
    src, dest = sys.argv[1:]
    src = src.replace('\\', '/')
    shutil.copy(src, dest)
