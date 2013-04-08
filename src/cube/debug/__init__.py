# -*- encoding: utf-8 -*-


import inspect

def _section(kind, name):
    frame = inspect.stack(3)[2]
    module = inspect.getmodule(frame[0].f_code).__name__
    args_infos = inspect.getargvalues(frame[0])
    file_, line, function = frame[1], frame[2], frame[3]
    if args_infos.args and args_infos.args[0] == 'self':
        args_infos.args.pop(0)
        cls = args_infos.locals['self'].__class__.__name__ #.split('.')[-1]
        module = "%s.%s" % (module, cls)
    function = "%s(%s)" % (
        function,
        ', '.join(
            str(k) for k in args_infos.args
        ),
    )
    return kind(
        name, file_, line, module + '.' + function,
    )

    return _section(Section, name)


def check_performance(name):
    from .performance import Section
    def performance_decorator(function):
        section = Section(name, "file", 0, function.__name__)
        def wrapper(*args, **kwargs):
            section.__enter__()
            try:
                return function(*args, **kwargs)
            finally:
                section.__exit__(None, None, None)
        return wrapper
    return performance_decorator
