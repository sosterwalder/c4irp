#!/usr/bin/env bash

source /etc/profile
pyenv local 3.5.2
make test-all
