"""Setuptools package definition"""

# pylint: disable=exec-used
from setuptools import setup
from setuptools import find_packages
import sys
import os

base_dir = os.path.dirname(os.path.realpath(__file__))

version = sys.version_info[0]
if version > 2:
    pass
else:
    pass

__version__  = None
version_file = "c4irp/version.py"
with open(version_file) as f:
    code = compile(f.read(), version_file, 'exec')
    exec(code)


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

with open('README.rst', 'r') as f:
    README_TEXT = f.read()

setup(
    name = "c4irp",
    version = __version__,
    packages = find_packages(),
    package_data=find_data(
        find_packages(), ["so"]
    ),
    entry_points = {
        'console_scripts': [
        ]
    },
    install_requires = [
        "cffi",
    ],
    setup_requires = [
        "cffi",
    ],
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
