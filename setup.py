"""Setuptools package definition"""

# pylint: disable=exec-used
from setuptools import setup
from setuptools import find_packages
from setuptools.command.install import install
import codecs
import os
import sys

base_dir = os.path.dirname(os.path.realpath(__file__))
CFLAGS = [
    "-O3",
    "-DNDEBUG",
    "-Wall",
    "-Werror",
    "-Wno-unused-function",
    "-Ilibuv/include",
    "-fPIC",
]

os.environ["SETUPCFLAGS"] = (
    " ".join(CFLAGS)
)

requires = [
    "cffi",
]
if sys.version_info < (3, 2):
    requires.append("futures")

__version__  = None
version_file = "c4irp/version.py"
with codecs.open(version_file, encoding="UTF-8") as f:
    code = compile(f.read(), version_file, 'exec')
    exec(code)


class CustomInstallCommand(install):
    """CustomInstallCommand"""
    def run(self):
        if sys.platform == "win32":
            os.system("cmd /C makefile.cmd release")
        else:
            os.system('make -f make.release')
        install.run(self)


def find_data(packages, extensions):
    """Finds data files along with source.

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
    version = __version__,
    cffi_modules=["chirp_cffi/high_level.py:ffi"],
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
    ],
    cmdclass = {
        'install': CustomInstallCommand,
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
