
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


from unittest import TestCase

class _(TestCase):

    def setUp(self):
        import tempfile
        self.tmp = tempfile.mkdtemp()
        self.app = Application(self.tmp, ('127.0.0.1', 0))
        self.thread = threading.Thread(target = self.app.run)

    def start_server(self):
        import time
        self.thread.start()
        time.sleep(.1)

    def tearDown(self):
        import shutil
        self.app.stop()
        if self.thread.is_alive():
            self.thread.join()
        shutil.rmtree(self.tmp, ignore_errors = True)

    def test_has_port(self):
        self.start_server()
        self.assertTrue(self.app.server_address[1] != 0)
        print(self.app.server_address)

    def test_directory(self):
        self.start_server()
        self.assertEqual(self.tmp, self.app.directory)

    def test_get_frameworks_empty(self):
        self.assertEqual(len(list(self.app.get_frameworks())), 0)
        self.start_server()
        self.assertEqual(len(list(self.app.get_frameworks())), 0)



    def test_get_frameworks_ignore_non_zip_files(self):
        with open(os.path.join(self.tmp, 'test'), 'w'): pass
        with open(os.path.join(self.tmp, 'test.lol'), 'w'): pass
        with open(os.path.join(self.tmp, 'zip.test.lol'), 'w'): pass
        with open(os.path.join(self.tmp, 'zip.test.ZIP'), 'w'): pass
        self.assertEqual(len(list(self.app.get_frameworks())), 0)

    def __create_dummy_release(self, name, compiler = 'CC', type = 'DEBUG', manifest = {}):
        with zipfile.ZipFile(os.path.join(self.tmp, name), 'w') as f:
            f.writestr(type + '-' + compiler + '/.manifest', json.dumps(manifest))

    def test_get_frameworks_ignore_malformed_zip_files(self):
        self.__create_dummy_release('a.zip')
        self.__create_dummy_release('a-b.zip')
        self.__create_dummy_release('a-b-c.zip')
        self.__create_dummy_release('a-b-c-d.zip')
        self.__create_dummy_release('a-b-c-d-e.zip')
        # 6 and 7 parts are valid
        self.__create_dummy_release('a-b-c-d-e-f-g-h.zip')
        self.assertEqual(len(list(self.app.get_frameworks())), 0)

    def test_get_framework(self):
        self.__create_dummy_release(
            'OS-ARCH-TYPE-COMPILER-VERSION-42-SHA1.zip',
            compiler = 'COMPILER',
            type = 'TYPE',
        )
        frameworks = list(self.app.get_frameworks())
        self.assertEqual(len(frameworks), 1)
        f = frameworks[0]
        self.assertEqual(f['os'], 'OS')
        self.assertEqual(f['arch'], 'ARCH')
        self.assertEqual(f['type'], 'TYPE')
        self.assertEqual(f['compiler'], 'COMPILER')
        self.assertEqual(f['version'], 'VERSION')
        self.assertEqual(f['build'], 42)
        self.assertEqual(f['sha1'], 'SHA1')

    def test_get_framework_exact_tag(self):
        self.__create_dummy_release(
            'OS-ARCH-TYPE-COMPILER-VERSION-SHA1.zip',
            compiler = 'COMPILER',
            type = 'TYPE',
        )
        frameworks = list(self.app.get_frameworks())
        self.assertEqual(len(frameworks), 1)
        f = frameworks[0]
        self.assertEqual(f['os'], 'OS')
        self.assertEqual(f['arch'], 'ARCH')
        self.assertEqual(f['type'], 'TYPE')
        self.assertEqual(f['compiler'], 'COMPILER')
        self.assertEqual(f['version'], 'VERSION')
        self.assertEqual(f['build'], 0)
        self.assertEqual(f['sha1'], 'SHA1')

    def __get(self, url, **kw):
        kw.setdefault('os', 'OS')
        kw.setdefault('type', 'TYPE')
        kw.setdefault('arch', 'ARCH')
        import json
        from http.client import HTTPConnection as Client
        c = Client(*self.app.server_address)
        import urllib
        c.request('GET', url + '?' + urllib.parse.urlencode(list(kw.items())))
        r = c.getresponse()
        self.assertEqual(r.status, 200)
        return json.loads(r.read().decode('utf8'))

    def test_get_status(self):
        self.start_server()
        self.assertEqual(self.__get('/')['status'], 'ok')

    def test_get_last_version(self):
        self.__create_dummy_release(
            'OS-ARCH-TYPE-COMPILER-VERSION-SHA1.zip',
            compiler = 'COMPILER',
            type = 'TYPE',
        )
        self.start_server()
        last_version = self.__get('/manifest')
        self.assertEqual(last_version['os'], 'OS')
        self.assertEqual(last_version['arch'], 'ARCH')
        self.assertEqual(last_version['type'], 'TYPE')
        self.assertEqual(last_version['compiler'], 'COMPILER')
        self.assertEqual(last_version['version'], 'VERSION')
        self.assertEqual(last_version['build'], 0)
        self.assertEqual(last_version['sha1'], 'SHA1')

    def test_ordered_tag_last_version(self):
        for tag in ['0.1', '1.2', '2.1', '2.0']:
            self.__create_dummy_release(
                'OS-ARCH-TYPE-COMPILER-%s-SHA1.zip' % tag,
                compiler = 'COMPILER',
                type = 'TYPE',
            )
        self.start_server()
        last_version = self.__get('/manifest')
        self.assertEqual(last_version['version'], '2.1')

    def test_ordered_build_last_version(self):
        for build in [10, 42, 40, 12, 5]:
            self.__create_dummy_release(
                'OS-ARCH-TYPE-COMPILER-0.1-%s-SHA1.zip' % build,
                compiler = 'COMPILER',
                type = 'TYPE',
            )
        self.start_server()
        last_version = self.__get('/manifest')
        self.assertEqual(last_version['build'], 42)

    def test_ordered_tag_and_build_last_version(self):
        for tag, build in [('0.1', 10), ('0.2', 42), ('1.1', 12), ('0.3', 12)]:
            self.__create_dummy_release(
                'OS-ARCH-TYPE-COMPILER-%s-%s-SHA1.zip' % (tag, build),
                compiler = 'COMPILER',
                type = 'TYPE',
            )
        self.start_server()
        last_version = self.__get('/manifest')
        self.assertEqual(last_version['version'], '1.1')
        self.assertEqual(last_version['build'], 12)

    def test_last_version_for_os(self):
        for os, tag, build in [('win', '0.1', 10), ('linux', '0.2', 42), ('osx', '1.1', 12), ('linux', '0.3', 12)]:
            self.__create_dummy_release(
                '%s-ARCH-TYPE-COMPILER-%s-%s-SHA1.zip' % (os, tag, build),
                compiler = 'COMPILER',
                type = 'TYPE',
            )
        self.start_server()
        last_version = self.__get('/manifest', os = 'win')
        self.assertEqual(last_version['version'], '0.1')
        self.assertEqual(last_version['build'], 10)

        last_version = self.__get('/manifest', os = 'linux')
        self.assertEqual(last_version['version'], '0.3')
        self.assertEqual(last_version['build'], 12)

        last_version = self.__get('/manifest', os = 'osx')
        self.assertEqual(last_version['version'], '1.1')
        self.assertEqual(last_version['build'], 12)
