#!/usr/bin/env bash

set -e

if [ "$TRAVIS_PYTHON_VERSION" == "pypy" ]; then
	sudo rm -rf /opt/pyenv
	sudo rm -rf $HOME/.pyenv
	curl -L https://raw.githubusercontent.com/yyuu/pyenv-installer/master/bin/pyenv-installer | bash
fi
