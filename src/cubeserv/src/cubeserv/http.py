
from http.server import HTTPServer, BaseHTTPRequestHandler
import threading

class RequestHandler(BaseHTTPRequestHandler):
    pass

class Locked:

    def __init__(self, value = None):
        self.__value = value
        self.__lock = threading.Lock()

    def __enter__(self):
        self.__lock.__enter__()
        return self.__value

    def __exit__(self, *args):
        self.__lock.__exit__(*args)

    def reset(self, value):
        with self:
            self.__value = value


class Server(HTTPServer):
    def __init__(self, address):
        super().__init__(address, RequestHandler)
        self.__frameworks = Locked(tuple())

    def run(self):
        print("Listening on %s:%s" % self.server_address)
        self.serve_forever()

    @property
    def frameworks(self):
        with self.__frameworks as l:
            return l[:]

    @frameworks.setter
    def frameworks(self, list):
        list = tuple(list)
        print("Update framework list:", list)
        self.__frameworks.reset(list)
