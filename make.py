#!/usr/bin/env python
"""OS independent entry point for the project."""
import os
import sys
import shutil

dotfiles = [
    "syntastic_c_config",
    "coafile",
    "floo",
    "flooignore",
    "clang_complete",
]

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


def make_dotfiles(files):
    """Copy the dot-files if they are missing."""
    for file_ in files:
        target = ".%s" % file_
        if not os.path.exists(target):
            source = os.path.join("build", file_)
            make_file(source, target)


def make_file(source, target):
    """Symlink or copy a file."""
    if sys.platform == "win32":
        shutil.copy(source, target)
    else:
        os.symlink(source, target)

make_dotfiles(dotfiles)
make_file(os.path.join("build", "pytest.ini"), "pytest.ini")

if sys.platform == "win32":
    pass
else:
    try:
        os.symlink("build/pyproject", "pyproject")
    except Exception:  # Well, lazy python 2/3 compat, really I don't care
        pass
    os.execvp("make", ["make", "-f", "build/project.make"] + sys.argv[1:])
