
import os
import threading
import time

from . import http

class Application:

    def __init__(self, directory, address):
        self.directory = os.path.abspath(directory)
        self.address = self._split_address(address)
        self.running = False

    def run(self):
        self.running = True
        thread = threading.Thread(target = self.__update_list)
        try:
            self.server = http.Server(self.address)
            thread.start()
            self.server.run()
        finally:
            self.running = False
            thread.join()

    def __update_list(self, period = 10):
        while self.running:
            self.server.frameworks = self.get_frameworks()
            slept = 0
            start = time.time()
            while slept < period and self.running:
                time.sleep(.01)
                slept = time.time() - start

    def get_frameworks(self):
        files = os.listdir(self.directory)
        for f in files:
            yield {
                'file': os.path.join(self.directory, f),
            }

    @staticmethod
    def _split_address(addr):
        if isinstance(addr, tuple):
            pass
        elif isinstance(addr, str):
            if ':' in addr:
                host, port = addr.split(':')
            else:
                host, port = 'localhost', addr
            addr = (host, int(port))
        else:
            raise TypeError(
                "Got unexpected type {}, expect str or tuple".format(type(addr))
            )

        assert isinstance(addr[0], str)
        assert isinstance(addr[1], int)
        assert len(addr) == 2
        return addr
