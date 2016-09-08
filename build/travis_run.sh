#!/usr/bin/env bash

set -e

./make.py > /dev/null
if [ "$BUILDTYPE" == "osx-test" ]; then
    brew update
    brew install libffi cppcheck openssl
    NOLIB=true sudo ./make.py test
elif [ "$BUILDTYPE" == "osx-lib" ]; then
    brew update
    brew install libffi cppcheck openssl
    NOLIB=false sudo ./make.py test-lib
elif [ "$BUILDTYPE" == "osx-release" ]; then
    brew update
    brew install libffi cppcheck openssl
    sudo python setup.py install
else
    docker run -v $PWD:/outside adfinissygroup/chirp-jessie /bin/sh \
        -c "cd /outside && ./build/docker_run.sh $BUILDTYPE"
fi
