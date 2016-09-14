"""Setuptools package definition."""

from distutils.command import build_ext
from setuptools import setup
from setuptools import find_packages
import codecs
import os
import sys

requires = [
    "six",
]
if sys.version_info < (3, 2):
    requires.append("futures")
cffi_modules = ["chirp_cffi/high_level.py:ffi"]
if '-g' in sys.argv:
    os.environ['MODE'] = "debug"
if 'CC' not in os.environ:
    if sys.platform != "win32":
        # Set the same compiler python uses
        import distutils.sysconfig
        import distutils.ccompiler
        compiler = distutils.ccompiler.new_compiler()
        distutils.sysconfig.get_config_var('CUSTOMIZED_OSX_COMPILER')
        distutils.sysconfig.customize_compiler(compiler)
        os.environ['CC'] = compiler.compiler_so[0]
if 'MODE' not in os.environ:
    os.environ['MODE'] = "release"
if os.environ['MODE'].lower() == "debug":
    cffi_modules.append("chirp_cffi/low_level.py:ffi")
else:
    os.environ['MODE'] = "release"


version_file = "chirp/version.py"
version = {}
with codecs.open(version_file, encoding="UTF-8") as f:
    code = compile(f.read(), version_file, 'exec')
    exec(code, version)


class Build(build_ext.build_ext):
    """Custom build class to build chirp before building the cffi extension."""

    def run(self):
        if sys.platform == "win32":
            ret = os.system("python make.py")
        else:
            ret = os.system('make -f build/build.make')
        if ret != 0:
            raise OSError("Chirp build failed")
        build_ext.build_ext.run(self)


def find_data(packages, extensions):
    """Find data files along with source.

    :param   packages: Look in these packages
    :param extensions: Look for these extensions
    """
    data = {}
    for package in packages:
        package_path = package.replace('.', '/')
        for dirpath, _, filenames in os.walk(package_path):
            for filename in filenames:
                for extension in extensions:
                    if filename.endswith(".%s" % extension):
                        file_path = os.path.join(
                            dirpath,
                            filename
                        )
                        file_path = file_path[len(package) + 1:]
                        if package not in data:
                            data[package] = []
                        data[package].append(file_path)
    return data

with codecs.open('README.rst', 'r', encoding="UTF-8") as f:
    README_TEXT = f.read()

setup(
    name = "c4irp",
    version = version['__version__'],
    cffi_modules=cffi_modules,
    packages = find_packages(),
    package_data=find_data(
        find_packages(), ["pem"]
    ),
    entry_points = {
        'console_scripts': [
        ]
    },
    install_requires = requires,
    setup_requires = [
        "cffi",
        "requests",
    ],
    cmdclass = {
        'build_ext': Build,
    },
    author = "Jean-Louis Fuchs",
    author_email = "ganwell@fangorn.ch",
    description = "Message-passing and actor-based programming for everyone",
    long_description = README_TEXT,
    keywords = "chirp, message-passing, actor-based, c-implementation",
    url = "https://concretecloud.github.io",
    classifiers = [
        "Development Status :: 4 - Beta",
        "Environment :: Console",
        "Intended Audience :: Developers",
        "Intended Audience :: Information Technology",
        "License :: OSI Approved :: "
        "GNU Affero General Public License v3",
        "Natural Language :: English",
        "Operating System :: OS Independent",
        "Programming Language :: Python :: 2.7",
        "Programming Language :: Python :: 3.4",
        "Programming Language :: Python :: 3.5",
        "Programming Language :: Python :: Implementation :: CPython",
        "Programming Language :: Python :: Implementation :: PyPy",
    ]
)
