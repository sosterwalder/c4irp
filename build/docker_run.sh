#!/usr/bin/env bash

set -e
export BUILDTYPE="$1"
export CHIRP_DOCKER="True"

source /etc/profile
pyenv local 3.5.2
./make.py > /dev/null
export NOLIB=true
if [ "$BUILDTYPE" == "lib" ]; then
    NOLIB=false ./make.py test-lib
elif [ "$BUILDTYPE" == "doc" ]; then
    ./make.py doc-all
elif [ "$BUILDTYPE" == "bdist" ]; then
    pip install wheel
    python setup.py bdist_wheel
else
    pyenv local $BUILDTYPE
    ./make.py test
fi
