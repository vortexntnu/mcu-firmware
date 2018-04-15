#!/bin/bash

rsync -r --quiet $TRAVIS_BUILD_DIR/ vortex@odroid.vortexntnu.no:/home/vortex/motor-control-interface

# TODO: Flash MCU

ssh vortex@odroid.vortexntnu.no pytest -v /home/vortex/motor-control-interface/test
