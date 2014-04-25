
import json
import os
import threading
import time
import zipfile

from . import http

class Application:

    def __init__(self, directory, address):
        self.__directory = os.path.abspath(directory)
        self.__address = self._split_address(address)
        self.__running = False

    def run(self):
        self.__running = True
        thread = threading.Thread(target = self.__update_list)
        try:
            self.server = http.Server(self.__address)
            thread.start()
            self.server.run()
        finally:
            self.__running = False
            thread.join()

    def stop(self):
        if self.__running:
            self.__running = False
            self.server.shutdown()

    @property
    def server_address(self):
        if not self.__running:
            raise Exception("Not running")
        return self.server.server_address

    @property
    def directory(self):
        return self.__directory

    def __update_list(self, period = 10):
        while self.__running:
            self.server.frameworks = self.get_frameworks()
            slept = 0
            start = time.time()
            while slept < period and self.__running:
                time.sleep(.01)
                slept = time.time() - start


    def __get_framework(self, file):
            parts = file.split('-')
            if len(parts) == 7:
                os_, arch, type, compiler, version, build, sha1 = parts
            elif len(parts) == 6:
                os_, arch, type, compiler, version, sha1 = parts
                build = 0
            else:
                raise Exception("Not a valid file name")
            sha1 = sha1.split('.')[0] # remove .zip ext
            with zipfile.ZipFile(os.path.join(self.directory, file)) as f:
                with f.open(type + '-' + compiler + '/.manifest') as manifest_file:
                    manifest = json.loads(manifest_file.read().decode('utf8'))
            return {
                'file': file,
                'os': os_,
                'arch': arch,
                'type': type,
                'compiler': compiler,
                'version': version,
                'build': int(build),
                'sha1': sha1,
            }

    def get_frameworks(self):
        files = os.listdir(self.__directory)
        for f in files:
            if not f.endswith('.zip'):
                continue
            try:
                yield self.__get_framework(f)
            except Exception as e:
                print("Ignore", f, ':', e)
                pass

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
