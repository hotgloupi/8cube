import json
import threading
import urllib

from http.server import HTTPServer, BaseHTTPRequestHandler

class RequestHandler(BaseHTTPRequestHandler):

    def __manifest(self, os,
                   os_version = 'last',
                   version = 'last',
                   arch = '32bit',
                   type = 'release'):
        all = (
            f for f in self.server.frameworks
            if f['arch'] == arch and f['type'] == type and f['os'] == os
        )

        return sorted(
            all,
            key = lambda f: (f['version'], f['build'])
        )[-1]

    def __status(self, **kw):
        return {
            'status': 'ok',
        }

    mapping = {
        '/manifest': __manifest,
        '/': __status,
    }

    def do_GET(self):
        if '?' in self.path:
            path, kw = self.path.split('?')
            kw = dict(urllib.parse.parse_qsl(kw))
        else:
            path, kw = self.path, {}

        code = 200
        try:
            res = json.dumps(self.mapping[path](self, **kw))
        except Exception as e:
            self.log_error(str(e))
            print("LOL", e)
            code = 500
            res = json.dumps(str(e))
        self.send_response(code)
        self.end_headers()
        self.wfile.write(res.encode('utf-8'))

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
