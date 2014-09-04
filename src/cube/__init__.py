# -*- encoding: utf-8 -*-

"""A cross-platform GUI application framework.

The cube library provides:
    * a (portable) graphic library (cube.gl)
    * a GUI framework (cube.gui)
    * a system abstraction (cube.system)

"""
from . import converters

from .exception import Exception
from .debug import check_performance
from .log import \
    debug, info, warn, error, fatal, \
    trace, trace_info, trace_warn, trace_error, trace_fatal

from .application import Application
from . import constants, resource, system, gl, gui, scene, test

