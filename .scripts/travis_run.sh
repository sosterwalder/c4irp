#!/usr/bin/env bash

if [ "$BUILDTYPE" == "osx" ]; then
    brew install libffi
    sudo make test-all
else
    docker run -v $PWD:/outside adfinissygroup/chirp-jessie /bin/sh \
        -c "cd /outside && ./.scripts/docker_run.sh $BUILDTYPE"
fi
