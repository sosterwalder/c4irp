#!/usr/bin/env bash

set -e

if [ "$BUILDTYPE" == "osx-test" ]; then
    brew install libffi cppcheck
    sudo ./make.py test
elif [ "$BUILDTYPE" == "osx-release" ]; then
    brew install libffi
    sudo python setup.py install
else
    docker run -v $PWD:/outside adfinissygroup/chirp-jessie /bin/sh \
        -c "cd /outside && ./.scripts/docker_run.sh $BUILDTYPE"
fi
