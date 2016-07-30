#!/usr/bin/env bash

source /etc/profile
pyenv local $PYTHON_ENV_VER
make test-all
