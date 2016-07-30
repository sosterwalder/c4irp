#!/usr/bin/env bash

BUILDTYPE="$1"

source /etc/profile
pyenv local 3.5.2
if [ "$BUILDTYPE" == "lib" ]; then
    make test-lib
elif [ "$BUILDTYPE" == "doc" ]; then
    make doc-all
elif [ "$BUILDTYPE" == "osx" ]; then
    make test-all
else
    pyenv local $BUILDTYPE
    make test-nolib
fi
