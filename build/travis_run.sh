#!/usr/bin/env bash

set -e

./make.py > /dev/null
if [ "$BUILDTYPE" == "osx-test" ]; then
    brew update
    brew uninstall libtool && brew install libtool
    brew install libffi cppcheck openssl
    sudo NOLIB=true ./make.py test
elif [ "$BUILDTYPE" == "osx-lib" ]; then
    brew update
    brew uninstall libtool && brew install libtool
    brew install libffi cppcheck openssl
    sudo NOLIB=false ./make.py test-lib
elif [ "$BUILDTYPE" == "osx-bdist" ]; then
    brew update
    brew uninstall libtool && brew install libtool
    brew install libffi cppcheck openssl
    sudo pip install wheel
    sudo python setup.py bdist_wheel
else
    docker run -v $PWD:/outside adfinissygroup/chirp-jessie /bin/sh \
        -c "cd /outside && ./build/docker_run.sh $BUILDTYPE"
fi
