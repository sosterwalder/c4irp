#!/usr/bin/env bash

set -e
make libchirp-depends
scan-build --status-bugs make libchirp.a
make clean
make test-all
make -f make.release clean all
pip install .
make doc-all
