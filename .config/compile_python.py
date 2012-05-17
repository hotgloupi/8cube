#!/usr/bin/env python3

import sys

if __name__ == "__main__":
    src, obj = sys.argv[1:]
    with open(src, 'r') as f:
        res = compile(f.read(), src, 'exec')
    with open(obj, 'wb') as f:
        f.write(res)
    print(src, obj)
