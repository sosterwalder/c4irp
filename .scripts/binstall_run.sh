#!/usr/bin/env bash

if [ ! "$BUILDTYPE" == "osx" ]; then
    docker pull adfinissygroup/chirp-jessie
fi
