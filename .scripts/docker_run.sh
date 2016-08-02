#!/usr/bin/env bash

set -e
BUILDTYPE="$1"

source /etc/profile
pyenv local 3.5.2
if [ "$BUILDTYPE" == "lib" ]; then
    make test-lib
elif [ "$BUILDTYPE" == "doc" ]; then
    make doc-all
elif [ "$BUILDTYPE" == "osx" ]; then
    make test-all
elif [ "$BUILDTYPE" == "bdist" ]; then
    make -f make.release
    python setup.py bdist
elif [ "$BUILDTYPE" == "release" ]; then
    python setup.py install
else
    pyenv local $BUILDTYPE
    make test-nolib
fi
