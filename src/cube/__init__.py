# -*- encoding: utf-8 -*-

"""A cross-platform GUI application framework.

The cube library provides:
    * a (portable) graphic library (cube.gl)
    * a GUI framework (cube.gui)
    * a system abstraction (cube.system)

"""

from .application import Application
from . import constants, gl, gui, system

from .exception import Exception
from .log import debug, info, warn, error, fatal
