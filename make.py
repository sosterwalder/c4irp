#!/usr/bin/env python
"""OS independent entry point for the project."""
import os
import sys

dotfiles = [
    "syntastic_c_config",
    "coafile",
    "floo",
    "flooignore",
    "clang_complete",
]

if 'CC' not in os.environ:
    # Set the same compiler python uses
    import distutils.sysconfig
    import distutils.ccompiler
    compiler = distutils.ccompiler.new_compiler()
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
            os.symlink(source, target)

make_dotfiles(dotfiles)

if sys.platform == "win32":
    pass
else:
    try:
        os.symlink("build/pyproject", "pyproject")
    except FileExistsError:
        pass
    os.execvp("make", ["make", "-f", "build/project.make"] + sys.argv[1:])
