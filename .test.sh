#!/usr/bin/env bash

set -e
make libc4irp-depends
scan-build --status-bugs make libc4irp.a
make clean
make test-all
make -f make.release
pip install .
make doc-all
cd /tmp && python -m c4irp.test_c4irp
