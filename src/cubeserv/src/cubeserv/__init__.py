
def isiterable(v):
    try: (e for e in v)
    except TypeError: return False
    return True

class Application:

    def __init__(self, *args, **kw):
        if len(args) == 1 and not kw and isiterable(args[0]):
            print("lol")

    def run(self):
        pass

