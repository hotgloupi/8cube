
# makes jedi-vim happy about print(*args) syntax
from __future__ import print_function

import datetime as dt
import hashlib
import json
import os
import shutil
import stat
import subprocess
import sys
import time
import tempfile
import zipfile

from os.path import join, isdir, isfile, dirname, abspath, exists, basename, relpath

SCRIPT = abspath(__file__)
SCRIPT_DIR = dirname(SCRIPT)
SCRIPT_CMD = sys.argv[0]
IS_WINDOWS = "win32" in sys.platform.lower()
IS_OSX = "darwin" in sys.platform.lower()

sys.path.append(join(SCRIPT_DIR, '.config', 'configure.py', 'src'))
from configure.tools import which

def fatal(*args, **kw):
    kw.update({'file': sys.stderr}, **kw)
    print(*args, **kw)
    sys.exit(1)

class Context(tempfile.TemporaryDirectory):
    def __init__(self, dirname):
        super().__init__()
        self.dirname = dirname

    def __enter__(self):
        self.work_dir = super().__enter__()
        self.dest_dir = join(self.work_dir, self.dirname)
        os.makedirs(self.dest_dir)
        self.log_file = open(join(self.dest_dir, ".release.log"), 'a')
        with open(join(self.dest_dir, '.release'), 'w') as f:
            print(dt.datetime.utcnow().ctime(), file = f)
        return self

    def __exit__(self, *args):
        self.log_file.flush()
        os.close(self.log_file.fileno())
        tries = 0
        while True:
            try:
                return super().__exit__(*args)
            except BaseException as e:
                print("Got exception:", e)
                tries += 1
                if tries == 10:
                    raise
                time.sleep(.1)


    def log(self, *args, **kw):
        silent = kw.pop('silent', False)
        if not silent:
            print(*args)
        print("->", *args, file = self.log_file)

    def debug(self, *args):
        self.log(*args, silent = True)

    def cmd_output(self, *args, **kwargs):
        self.debug('[COMMAND]', *args)
        return subprocess.check_output(list(args), **kwargs).decode('utf8').strip()

    def cmd(self, *args, **kwargs):
        self.debug('[COMMAND]', *args)
        return subprocess.check_call(
            list(args),
            stdout = self.log_file,
            stderr = subprocess.STDOUT,
            **kwargs
        )

    def is_binary(self, path):
        if IS_WINDOWS and any(path.lower().endswith(ext) for ext in ['.exe', '.dll', '.pyd']):
            return True
        if path.endswith('.py') or path.endswith('.sh') or path.endswith('.bat'):
            return False
        if not stat.S_IXUSR & os.stat(path)[stat.ST_MODE]:
            return False
        with open(path, 'rb') as f:
            if f.read(2) == b'#!':
                return False
        return True

    if which("otool"):
        def get_binary_dependencies(self, arg):
            deps = self.cmd_output('otool', '-L', arg).split('\n')[2:]
            for path in (dep.strip().split(" ")[0] for dep in deps):
                if path: yield path

    elif which("ldd"):
        def get_binary_dependencies(self, arg):
            deps = self.cmd_output('ldd', arg)
            deps = deps.split('\n')
            if not deps:
                return
            for dep in deps[1:]:
                dep = dep.strip()
                if not dep or '=>' not in dep:
                    return
                yield dep.split('=>')[1].strip().split(' ')[0].strip()
    elif not IS_WINDOWS:
        raise Exception("otool or ldd needed")

    if IS_OSX:
        def strip(self, bin):
            self.cmd('strip', '-S', '-x', bin)
    elif IS_WINDOWS:
        def strip(self, bin):
            self.cmd('strip', '-s', bin)
    else:
        def strip(self, bin):
            self.cmd('chmod', 'u+w', bin)
            self.cmd('strip', '-s', bin)


def copy_release_files(ctx, build_dir, dest_dir):
    ctx.log("Copying release files")
    build_release_dir = join(build_dir, 'release')
    for d in os.listdir(build_release_dir):
        ctx.log(" - Copying", join(build_release_dir, d), "in", dest_dir)
        shutil.copytree(join(build_release_dir, d), join(dest_dir, d))


def locate_python_home(ctx, build_dir):
    # XXX Find a better way
    for python_home in [
        join(build_dir, 'dependencies/Python34/gcc/release/no-pymalloc/install/lib/python3.4'),
        join(build_dir, 'dependencies/Python34/clang/release/no-pymalloc/install/lib/python3.4'),
        join(build_dir, 'dependencies/Python34/gcc/release/install/lib/python3.4'),
        join(build_dir, 'dependencies/Python34/clang/release/install/lib/python3.4'),
        "c:/Python34",
        None
    ]:
        if exists(python_home):
            return python_home
    raise Exception("Couldn't find python home")

def copy_python_files(ctx, python_home, dest_dir):
    dest_python_home = join(dest_dir, "lib/python3.4")
    remove_packages = [
        'distutils',
        'test',
        'turtledemo',
        'site-packages',
        'lib2to3',
    ]
    ctx.log("Copying python files in", dest_python_home)
    if IS_WINDOWS:
        os.makedirs(dest_python_home)
        for d in ['DLLs', 'Lib']:
            ctx.log(" - Copying", join(python_home, d), "in", join(dest_python_home, d))
            shutil.copytree(
                join(python_home, d),
                join(dest_python_home, d)
            )
        for p in remove_packages:
            p = join(dest_python_home, 'Lib', p)
            ctx.log(" - Removing package", p)
            shutil.rmtree(p, ignore_errors = True)
    else:
        shutil.copytree(python_home, dest_python_home)
        for p in remove_packages:
            p = join(dest_python_home, p)
            ctx.log(" - Removing package", p)
            shutil.rmtree(p, ignore_errors = True)

    if not IS_WINDOWS:
        # XXX Fix sysconfig on windows too ?
        with open(join(python_home, '_sysconfigdata.py'), 'r') as f:
            src = f.read()

        old_prefix = abspath(join(python_home, '../../..'))
        new_prefix = abspath(join(dest_python_home, '../../..'))
        # XXX still absolute path
        ctx.log('Fix', join(dest_python_home, '_sysconfigdata.py'), '(%s -> %s)' % (old_prefix, new_prefix))
        while old_prefix in src:
            src = src.replace(old_prefix, new_prefix)
            ctx.debug("Replace", old_prefix, new_prefix)
        dst = src
        with open(join(dest_python_home, '_sysconfigdata.py'), 'w') as f:
            f.write(dst)
        ctx.log("SYSCONFIG:", dst)
    return dest_python_home


def clean_release(ctx, dest_dir):
    ctx.log('Cleaning up the release dir')
    to_remove_ext = [
        '.o', '.a', '.pyc', '.lib', '~', '.depend.mk', '.command',
        '.ilk', '.pdb', '.exp',
    ]

    to_remove_inf = [
        '.timer.',
    ]

    to_remove_files = [
        'Tupfile',
    ]

    to_remove_dirs = [
        '__pycache__',
        '.dSYM'
    ]

    for root, dirs, files in os.walk(dest_dir):
        for f in files:
            if any(f.lower().endswith(ext) for ext in to_remove_ext) or \
               any(inf in f.lower() for inf in to_remove_inf) or \
               f in to_remove_files:
                ctx.log(" - Remove file", join(root, f))
                os.unlink(join(root, f))
                continue
        for d in dirs[:]:
            if any(d.endswith(ext) for ext in to_remove_dirs):
                dirs.remove(d)
                ctx.log(" - Remove directory", join(root, d))
                shutil.rmtree(join(root, d))

    if IS_WINDOWS:
        for f in os.listdir(join(dest_dir, 'lib')):
            if f.endswith('.dll'):
                p = join(dest_dir, 'lib', f)
                ctx.log(" - Remove useless dll", p)
                os.unlink(p)

def fix_unix_libraries(ctx, dest_dir):
    binaries = [
        binary.strip() for binary in ctx.cmd_output(
            'find', dest_dir, '-type', 'f', '-print'
        ).split('\n')
        if binary.strip() and ctx.is_binary(binary) and 'lib-dynload' not in binary
    ]


    def to_add(dep):
        return not (
            #dep.startswith('/opt/local') or
            dep.startswith('/usr') or
            dep.startswith('/lib') or
            dep.startswith('/System') or
            dep.startswith(dest_dir)
        ) or 'libstdc++' in dep or 'libgcc' in dep

    def get_binary_rpaths(binary_path):
        lines = (l.strip() for l in ctx.cmd_output('otool', '-l', binary_path).split('\n'))
        for l in lines:
            if l.startswith('path '):
                yield l.split(' ')[1]

    def solve_rpath_dependency(binary_path, dependency):
        dep = '/'.join(dependency.split('/')[1:])
        for dir in ctx.get_binary_rpaths(binary_path):
            if exists(join(dir, dep)):
                return join(dir, dep)

    def fill_dependencies(dependencies, binary_path, indent=1):
        children = dependencies.setdefault(binary_path, set())
        l_dep = list(ctx.get_binary_dependencies(binary_path))
        ctx.debug('-' * indent, "Find dependencies of", binary_path, l_dep)
        for dependency in l_dep:
            if dependency.startswith('@rpath'):
                ctx.debug('-'*indent + "solve", dependency, "for", binary_path, "to",
                      solve_rpath_dependency(binary_path, dependency))
                dependency = solve_rpath_dependency(binary_path, dependency)
            elif dependency.startswith('@'):
                continue
            if not to_add(dependency):
                ctx.debug('-'*indent + '> IGNORE', dependency, "of", basename(binary_path))
                continue
            ctx.debug('-'*indent + '> Add dep', dependency, "of", basename(binary_path))
            children.add(dependency)
            if dependency not in dependencies:
                fill_dependencies(dependencies, dependency, indent + 1)

    dependencies = {}
    for bin in binaries:
        fill_dependencies(dependencies, bin)

    ctx.debug("Dumping dependencies:")
    for bin, deps in dependencies.items():
        ctx.debug(" * dependencies of %s:" % bin)
        for dep in deps:
            ctx.debug("    -", dep)

    dest_lib_dir = os.path.join(dest_dir, 'lib')
    shipped_dependencies = {}
    for k in dependencies.keys():
        if not k.startswith(dest_dir):
            dest = os.path.join(dest_lib_dir, os.path.basename(k))
            shipped_dependencies[k] = dest
            ctx.cmd('cp', '-fv', k, dest)

    ctx.debug("Shipped dependencies:", shipped_dependencies)

    if sys.platform.lower().startswith("darwin"):
        ctx.log("Fixup binaries dependencies")
        for dep in dependencies:
            if dep in shipped_dependencies:
                dep = shipped_dependencies[dep]
            cmd('chmod', 'u+w', dep)
            reldir = os.path.relpath(dest_lib_dir, start = os.path.dirname(dep))
            if dep.endswith('.so') or dep.endswith('.dylib'):
                ctx.cmd('install_name_tool', '-id', '@rpath/' + os.path.basename(dep), dep)
        rpath_added = {}
        for dep in dependencies:
            ctx.debug("Working on", dep)
            deps = dependencies[dep]
            if dep in shipped_dependencies:
                dep = shipped_dependencies[dep]
                debug("Which is copied at", dep)
            ctx.debug(dep, "has the follwing deps", deps)
            for subdep in deps:
                buildpath = shipped_dependencies[subdep]
                reldir = os.path.relpath(os.path.dirname(buildpath), start = os.path.dirname(dep))
                ctx.debug(dep, 'subdep', subdep, 'at', buildpath)
                ctx.cmd(
                    'install_name_tool',
                    '-change',
                    subdep,
                    '@rpath/' + os.path.basename(subdep),
                    dep
                )
                if reldir in rpath_added.get(dep, set()): continue
                rpath_added.setdefault(dep, set()).add(reldir)
                for path in get_binary_rpaths(dep):
                    ctx.cmd('install_name_tool', '-delete_rpath', path, dep)
                ctx.log(' - Add rpath', reldir, "to", dep)
                try:
                    ctx.cmd('install_name_tool', '-add_rpath', '@loader_path/' + reldir, dep)
                except:
                    ctx.log("Couldn't add rpath", reldir, "to", dep)
    else:
        for dep, subdeps in dependencies.items():
            ctx.debug("Working on", dep)
            if dep in shipped_dependencies:
                print(dep, shipped_dependencies[dep])
                dep = shipped_dependencies[dep]
            subdeps = set(shipped_dependencies[s] for s in subdeps)
            dep_dir = os.path.dirname(dep)
            reldirs = set(map(
                lambda lib: os.path.relpath(os.path.dirname(lib), start = dep_dir),
                subdeps
            ))
            ctx.log(' - Set rpath', ' '.join(reldirs), 'to', dep)
            ctx.cmd(
                'patchelf',
                #'--debug',
                '--set-rpath', ':'.join(os.path.join('$' + 'ORIGIN', d) for d in reldirs),
                dep
            )

def fix_windows_libraries(ctx, dest_dir):
    ctx.log(" - Copying redistributables")
    shutil.copy(which('msvcr120.dll'), join(dest_dir, 'bin'))
    shutil.copy(which('msvcp120.dll'), join(dest_dir, 'bin'))
    shutil.copy(which('python34.dll'), join(dest_dir, 'bin'))

def fix_libraries(ctx, dest_dir):
    if IS_WINDOWS:
        fix_windows_libraries(ctx, dest_dir)
    else:
        fix_unix_libraries(ctx, dest_dir)


def strip_binaries(ctx, dest_dir, python_home):
    ctx.log("Stripping release files")
    to_strip = []
    for root, dirs, files in os.walk(dest_dir):
        for f in files:
            path = join(root, f)
            if ctx.is_binary(path):
                to_strip.append(path)

    for bin in to_strip:
        if abspath(bin).startswith(abspath(python_home)):
            ctx.debug(" - Ignore", bin)
            continue
        try:
            ctx.log(" - Stripping", bin)
            ctx.strip(bin)
        except:
            ctx.log("Couldn't strip", bin)

def create_manifest(ctx, dest_dir):
    ctx.log("Create manifest")

    def hash(p):
        sha = hashlib.sha256()
        size = 0
        with open(p, 'rb') as f:
            data = f.read(4096)
            while data:
                size += len(data)
                sha.update(data)
                data = f.read(4096)
        return (size, sha.hexdigest())

    manifest = {'files': {}}

    for root, dirs, files in os.walk(dest_dir):
        for f in files:
            path = join(root, f)
            manifest['files'][path] = hash(path)
    manifest['size'] = sum(v[0] for v in manifest['files'].values())
    manifest['revision'] = ctx.cmd_output('git', 'rev-parse', 'HEAD')

    with open(join(dest_dir, '.manifest'), 'w') as f:
        json.dump(manifest, f)

    ctx.log("Ending release at", dt.datetime.utcnow().ctime())
    ctx.log("Total size", ctx.cmd_output('du', '-hs', dest_dir))

    ctx.log("Packaged files:")
    for f, h in manifest['files'].items():
        ctx.log("\t%s: %s" % (f, h[0]))
    ctx.log("Revision", manifest['revision'])
    ctx.log('Total size', manifest['size'])
    return manifest


def make_tarball(ctx, dir, output_file):
    with open(join(dir, '.manifest')) as f:
        manifest = json.load(f)
    root_dir = join(dir, '..')
    with zipfile.ZipFile(output_file, mode = 'w', compression = zipfile.ZIP_DEFLATED) as zip:
        old = os.getcwd()
        os.chdir(root_dir)
        try:
            zip.write(relpath(join(dir, '.manifest'), start = root_dir))
            for f in manifest['files'].keys():
                zip.write(relpath(f, start = root_dir))
        finally:
            os.chdir(old)
    ctx.log("Created tarball", output_file, 'of', ctx.cmd_output('du', '-hs', output_file))

def make_release(build_dir, output_file):
    if not isdir(build_dir):
        fatal("Error: ", build_dir, ": not a directory")
    with Context(output_file.split('.')[0]) as ctx:
        ctx.log("Starting release of ", build_dir, "in", ctx.dest_dir, 'at', dt.datetime.utcnow().ctime())
        copy_release_files(ctx, build_dir, ctx.dest_dir)
        python_home = copy_python_files(ctx, locate_python_home(ctx, build_dir), ctx.dest_dir)
        clean_release(ctx, ctx.dest_dir)
        fix_libraries(ctx, ctx.dest_dir)
        strip_binaries(ctx, ctx.dest_dir, python_home)
        manifest = create_manifest(ctx, ctx.dest_dir)
        make_tarball(ctx, ctx.dest_dir, output_file)
    return 0


if __name__ == '__main__':
    sys.exit(make_release(sys.argv[1], sys.argv[2]))
