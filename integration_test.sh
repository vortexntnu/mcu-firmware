#!/bin/bash

rsync -r  $TRAVIS_BUILD_DIR/ -e "ssh -p 2222" vortex@odroid.vortexntnu.no:/home/vortex/motor-control-interface

# TODO: Flash MCU

ssh -p 2222 vortex@odroid.vortexntnu.no "python3 -m pytest -v /home/vortex/motor-control-interface/test"
