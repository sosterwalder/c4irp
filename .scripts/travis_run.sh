#!/usr/bin/env bash

docker run -v $PWD:/outside adfinissygroup/chirp-jessie /bin/sh \
    -c "cd /outside && ./.scripts/docker_run.sh $BUILDTYPE"
