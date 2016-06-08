#!/usr/bin/env bash

set -e

if [ "$TRAVIS_PYTHON_VERSION" == "pypy" ]; then
	pyenv update
	pyenv install pypy-5.1.1
	pyenv global pypy-5.1.1
fi
