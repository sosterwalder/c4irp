#!/usr/bin/env python
"""OS independent entry point for the project."""
import os
import sys
import shutil

winssl = (
    "https://github.com/concretecloud/winopenssl"
    "/releases/download/CC1.0.2h_c1"
)

dotfiles = [
    "syntastic_c_config",
    "coafile",
    "floo",
    "flooignore",
    "clang_complete",
    "requirements.txt",
]

if 'PYTHONPATH' not in os.environ and sys.platform == "win32":
    os.environ['PYTHONPATH'] = os.getcwd()

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
    os.environ['MODE'] = "debug"


def get_openssl(platform, c99):
    """Download openssl from release, checksum, and extract."""
    import requests
    import hashlib
    import codecs
    import tarfile
    name = "openssl-{year}-{bit}-{mode}".format(
        year = "2015" if c99 else "2008",
        bit =  "32" if platform == "x86" else "64",
        mode = os.environ['MODE'].lower()
    )
    filename = "%s.tar.bz2" % name
    url = "%s/%s" % (winssl, filename)
    sha = hashlib.sha256()
    print("Download %s" % url)
    with open(filename, 'wb') as handle:
        response = requests.get(url, stream=True)
        if not response.ok:
            raise RuntimeError("Cannot download %s" % url)

        for block in response.iter_content(2**14):
            sys.stdout.write("#")
            sys.stdout.flush()
            sha.update(block)
            handle.write(block)
    with codecs.open(
            os.path.join("build", "opensslsha256.sums"),
            'r',
            "UTF-8"
    ) as f:
        hashes = {line.split(' ')[0].lower() for line in f.readlines()}
    digest = sha.hexdigest()
    if digest not in hashes:
        raise RuntimeError(
            "Cannot verify openssl %s not in digest list" % digest
        )
    with tarfile.open(filename, 'r:bz2') as f:
        f.extractall()
    os.unlink(filename)
    os.rename(name, "openssl")


def make_dotfiles(files):
    """Copy the dot-files if they are missing."""
    for file_ in files:
        target = ".%s" % file_
        source = os.path.join("build", file_)
        make_file(source, target)


def make_file(source, target):
    """Symlink or copy a file."""
    if not os.path.exists(target):
        if sys.platform == "win32":
            shutil.copy(source, target)
        else:
            os.symlink(source, target)

make_dotfiles(dotfiles)
make_file(os.path.join("build", "pytest.ini"), "pytest.ini")

if sys.platform == "win32":
    make_file(os.path.join("build", "config.defs.h"), "config.h")
    c99 =  sys.version_info > (3, 4)
    import platform
    if "64bit" in platform.architecture()[0].lower():
        pf = "x64"
    else:
        pf = "x86"
    if not os.path.exists("openssl"):
        get_openssl(pf, c99)
    if c99:
        os.system(" ".join([
            "build\\winbuildlibs2015.cmd %s" % pf
        ] + sys.argv[1:]))
    else:
        os.system(" ".join([
            "build\\winbuildlibs2008.cmd %s" % pf
        ] + sys.argv[1:]))
else:
    try:
        os.symlink("build/pyproject", "pyproject")
    except Exception:  # Well, lazy python 2/3 compat, really I don't care
        pass
    os.execvp("make", ["make", "-f", "build/project.mk"] + sys.argv[1:])
