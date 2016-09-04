#!/usr/bin/env python
"""OS independent entry point for the project."""
import os
import shutil

dotfiles = [
    "syntastic_c_config",
    "coafile",
    "floo",
    "flooignore",
    "clang_complete",
]


def make_dotfiles(files):
    """Copy the dot-files if they are missing."""
    for file_ in files:
        target = ".%s" % file_
        if not os.path.exists(target):
            source = os.path.join("build", file_)
            shutil.copy(source, target)

make_dotfiles(dotfiles)
