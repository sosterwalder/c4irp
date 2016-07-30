#!/usr/bin/env bash

docker run -v $PWD:/outside adfinissygroup/chirp-jessie /bin/sh \
    -c "cd /outside && pyenv local 3.5.2 make test-all"
