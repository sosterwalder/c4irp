#!/usr/bin/env bash

set -e
make libc4irp-depends
scan-build make libc4irp.a
make test-all
