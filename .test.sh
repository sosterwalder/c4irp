#!/usr/bin/env bash

set -e
make libc4irp-depends
scan-build --status-bugs make libc4irp.a
make clean
make test-all
make -f make.release clean all
pip install .
make doc-all
