#!/bin/sh
cd `dirname "$0"`
python3 -munittest discover -t src -p '*.py' -s src/cubeserv
