# -*- encoding: utf-8 -*-

from . import _log
from ._log import Mode, set_mode
import inspect

def _send(level, *msgs):
    frame = inspect.stack(3)[2]
    module = inspect.getmodule(frame[0].f_code)
    if module is not None:
        module = module.__name__
    else:
        module = '<unknown>'
    args_infos = inspect.getargvalues(frame[0])
    file_, line, function = frame[1], frame[2], frame[3]
    if args_infos.args and args_infos.args[0] == 'self':
        args_infos.args.pop(0)
        cls = args_infos.locals['self'].__class__.__name__ #.split('.')[-1]
        module = "%s.%s" % (module, cls)
    function = "%s(%s)" % (
        function,
        ', '.join(
            "%s=%s" % (k, v) for k, v in (
                (k, args_infos.locals[k]) for k in args_infos.args
            )
        ),
    )
    return _log.send(
        level, file_, line, function, module,
        ' '.join(str(p) for p in msgs)
    )

def debug(*msgs):
    _send(_log.Level.debug, *msgs)

def info(*msgs):
    _send(_log.Level.info, *msgs)

def warn(*msgs):
    _send(_log.Level.warn, *msgs)

def error(*msgs):
    _send(_log.Level.error, *msgs)

def fatal(*msgs):
    _send(_log.Level.fatal, *msgs)

def _trace(level, *msgs):
    frame = inspect.stack(3)[2]
    module = inspect.getmodule(frame[0].f_code)
    if module is not None:
        module = module.__name__
    else:
        module = '<unknown>'
    args_infos = inspect.getargvalues(frame[0])
    file_, line, function = frame[1], frame[2], frame[3]
    if args_infos.args and args_infos.args[0] == 'self':
        args_infos.args.pop(0)
        cls = args_infos.locals['self'].__class__.__name__ #.split('.')[-1]
        module = "%s.%s" % (module, cls)
    function = "%s(%s)" % (
        function,
        ', '.join(
            "%s=%s" % (k, v) for k, v in (
                (k, args_infos.locals[k]) for k in args_infos.args
            )
        ),
    )
    return _log.Trace(
        level, file_, line, function, module, ' '.join(str(p) for p in msgs)
    )

def trace(*msgs):
    return _trace(_log.Level.debug, *msgs)

def trace_info(*msgs):
    return _trace(_log.Level.info, *msgs)

def trace_warn(*msgs):
    return _trace(_log.Level.warn, *msgs)

def trace_error(*msgs):
    return _trace(_log.Level.error, *msgs)

def trace_fatal(*msgs):
    return _trace(_log.Level.fatal, *msgs)
