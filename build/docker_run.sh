#!/usr/bin/env bash

set -e
BUILDTYPE="$1"

source /etc/profile
pyenv local 3.5.2
./make.py > /dev/null
NOLIB=true
if [ "$BUILDTYPE" == "lib" ]; then
    NOLIB=false ./make.py test-lib
elif [ "$BUILDTYPE" == "doc" ]; then
    ./make.py doc-all
elif [ "$BUILDTYPE" == "osx" ]; then
    ./make.py test
elif [ "$BUILDTYPE" == "bdist" ]; then
    python setup.py bdist
elif [ "$BUILDTYPE" == "release" ]; then
    python setup.py install
else
    pyenv local $BUILDTYPE
    ./make.py test
fi
