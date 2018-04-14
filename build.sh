#!/bin/bash

docker run -i --rm \
       -u $( id -u $USER ):$( id -g $USER ) \
       -v $(pwd):$(pwd) \
       -w $(pwd)/build \
       vortexntnu/mcu-fw-build:latest
